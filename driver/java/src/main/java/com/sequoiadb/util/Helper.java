/*
 * Copyright 2018 SequoiaDB Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
 * in compliance with the License. You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License
 * is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 * or implied. See the License for the specific language governing permissions and limitations under
 * the License.
 */

package com.sequoiadb.util;

import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

import org.bson.BSON;
import org.bson.BSONObject;
import org.bson.types.BSONDecimal;

import com.sequoiadb.exception.BaseException;
import com.sequoiadb.exception.SDBError;

public final class Helper {
    private Helper() {
    }

    public static final int ALIGN_SIZE = 4;

    public static String md5(String str) {
        MessageDigest md5;
        try {
            md5 = MessageDigest.getInstance("MD5");
        }
        catch (Exception e) {
            throw new BaseException(SDBError.SDB_SYS, e);
        }

        char[] charArray = str.toCharArray();
        byte[] byteArray = new byte[charArray.length];
        for (int i = 0; i < charArray.length; i++) {
            byteArray[i] = (byte) charArray[i];
        }

        byte[] md5Bytes = md5.digest(byteArray);
        StringBuilder hexValue = new StringBuilder();
        for (byte md5Byte : md5Bytes) {
            int val = (md5Byte) & 0xff;
            if (val < 16) {
                hexValue.append("0");
            }
            hexValue.append(Integer.toHexString(val));
        }

        return hexValue.toString();
    }

    public static int alignedSize(int original, int alignSize) {
        int size = original + alignSize - 1;
        size -= size % alignSize;
        return size;
    }

    public static int alignedSize(int original) {
        return alignedSize(original, ALIGN_SIZE);
    }

    public static byte[] encodeBSONObj(BSONObject obj) {
        return BSON.encode(obj);
    }

    public static BSONObject decodeBSONBytes(byte[] bytes) {
        return BSON.decode(bytes);
    }

    public static BSONObject decodeBSONObject(ByteBuffer in) {
        int position = in.position();
        int length = in.getInt(position);

        if (in.order() == ByteOrder.BIG_ENDIAN) {
            bsonEndianConvert(in.array(), position, length, false);
        }

        BSONObject obj = BSON.decode(in.array(), position);

        if (length < in.remaining()) {
            int alignedSize = alignedSize(length);
            in.position(alignedSize + position);
        }
        else {
            in.position(length + position);
        }

        return obj;
    }

    public static byte[] decodeBSONBytes(ByteBuffer in) {
        int position = in.position();
        int length = in.getInt(position);

        if (in.order() == ByteOrder.BIG_ENDIAN) {
            bsonEndianConvert(in.array(), position, length, false);
        }

        byte[] bytes = new byte[length];
        in.get(bytes);

        if (in.remaining() > 0) {
            int alignedSize = alignedSize(length);
            in.position(alignedSize + position);
        }

        return bytes;
    }

    public static int byteToInt(byte[] byteArray, boolean endianConvert) {
        if (byteArray == null) {
            throw new BaseException(SDBError.SDB_INVALIDARG);
        }

        ByteBuffer bb = ByteBuffer.wrap(byteArray);
        if (endianConvert) {
            bb.order(ByteOrder.LITTLE_ENDIAN);
        }
        else {
            bb.order(ByteOrder.BIG_ENDIAN);
        }

        return bb.getInt();
    }

    public static int byteToInt(byte[] array, int begin) {
        if (array == null) {
            throw new BaseException(SDBError.SDB_INVALIDARG);
        }
        ByteBuffer bb = ByteBuffer.allocate(4);
        int count = 0;
        while (count < 4) {
            bb.put(array[begin + count]);
            ++count;
        }
        return bb.getInt(0);
    }

    // l2r means "local to remote", when we build a message
    // for sending to remote, we set l2r to be true; when we
    // receive message from remote, and we need to handle endian,
    // we set l2r to be false
    public static void bsonEndianConvert(byte[] inBytes, int offset, int objSize, boolean l2r) {
        int begin = offset;
        arrayReverse(inBytes, offset, 4);
        offset += 4;
        byte type;
        while (offset < inBytes.length) {
            // get bson element type
            type = inBytes[offset];
            // move offset to next to skip type
            ++offset;
            if (type == BSON.EOO) {
                break;
            }
            // skip element name: '...\0'
            offset += getStrLength(inBytes, offset) + 1;
            switch (type) {
                case BSON.NUMBER:
                    arrayReverse(inBytes, offset, 8);
                    offset += 8;
                    break;
                case BSON.STRING:
                case BSON.CODE:
                case BSON.SYMBOL: {
                    // the first 4 bytes indicate the length of the string
                    // the length of the string is the real length plus 1('\0')
                    int length = Helper.byteToInt(inBytes, offset);
                    arrayReverse(inBytes, offset, 4);
                    int newLength = Helper.byteToInt(inBytes, offset);
                    offset += (l2r ? newLength : length) + 4;
                    break;
                }
                case BSON.OBJECT:
                case BSON.ARRAY: {
                    int length = getBsonLength(inBytes, offset, l2r);
                    bsonEndianConvert(inBytes, offset, length, l2r);
                    offset += length;
                    break;
                }
                case BSON.BINARY: {
                    int length = Helper.byteToInt(inBytes, offset);
                    arrayReverse(inBytes, offset, 4);
                    int newLength = Helper.byteToInt(inBytes, offset);
                    offset += (l2r ? newLength : length) + 5;
                    break;
                }
                case BSON.UNDEFINED:
                case BSON.NULL:
                case BSON.MAXKEY:
                case BSON.MINKEY:
                    break;
                case BSON.OID:
                    offset += 12;
                    break;
                case BSON.BOOLEAN:
                    offset += 1;
                    break;
                case BSON.DATE:
                    arrayReverse(inBytes, offset, 8);
                    offset += 8;
                    break;
                case BSON.REGEX:
                    // string regex
                    offset += getStrLength(inBytes, offset) + 1;
                    // string option
                    offset += getStrLength(inBytes, offset) + 1;
                    break;
                case BSON.REF: {
                    offset += 12;
                    // 4 bytes length + string + 12 bytes
                    int length = Helper.byteToInt(inBytes, offset);
                    arrayReverse(inBytes, offset, 4);
                    int newLength = Helper.byteToInt(inBytes, offset);
                    offset += (l2r ? newLength : length) + 4;
                    offset += 12;
                    break;
                }
                case BSON.CODE_W_SCOPE: {
                    // 4 bytes
                    arrayReverse(inBytes, offset, 4);
                    offset += 4;
                    // string
                    int length = Helper.byteToInt(inBytes, offset);
                    arrayReverse(inBytes, offset, 4);
                    int newLength = Helper.byteToInt(inBytes, offset);
                    offset += (l2r ? newLength : length) + 4;
                    // then object
                    int objLength = getBsonLength(inBytes, offset, l2r);
                    bsonEndianConvert(inBytes, offset, objLength, l2r);
                    offset += objLength;
                    break;
                }
                case BSON.NUMBER_INT:
                    arrayReverse(inBytes, offset, 4);
                    offset += 4;
                    break;
                case BSON.TIMESTAMP:
                    // 2 4-bytes
                    arrayReverse(inBytes, offset, 4);
                    offset += 4;
                    arrayReverse(inBytes, offset, 4);
                    offset += 4;
                    break;
                case BSON.NUMBER_LONG:
                    arrayReverse(inBytes, offset, 8);
                    offset += 8;
                    break;
                case BSON.NUMBER_DECIMAL: {
                    // size(4) + typemod(4) + dscale(2) + weight(2) + digits(2x)
                    // size
                    int length = Helper.byteToInt(inBytes, offset);
                    arrayReverse(inBytes, offset, 4);
                    int newLength = Helper.byteToInt(inBytes, offset);
                    offset += 4;
                    // typemod
                    arrayReverse(inBytes, offset, 4);
                    offset += 4;
                    // dscale
                    arrayReverse(inBytes, offset, 2);
                    offset += 2;
                    // weight
                    arrayReverse(inBytes, offset, 2);
                    offset += 2;
                    // digits
                    int ndigits = ((l2r ? newLength : length) - BSONDecimal.DECIMAL_HEADER_SIZE)
                            / (Short.SIZE / Byte.SIZE);
                    for (int i = 0; i < ndigits; i++) {
                        arrayReverse(inBytes, offset, 2);
                        offset += 2;
                    }
                    break;
                }

            }
        }
        if (offset - begin != objSize) {
            throw new BaseException(SDBError.SDB_INVALIDSIZE);
        }
    }

    private static int getBsonLength(byte[] inBytes, int offset, boolean endianConvert) {
        byte[] tmp = new byte[4];
        for (int i = 0; i < 4; i++) {
            tmp[i] = inBytes[offset + i];
        }

        return Helper.byteToInt(tmp, endianConvert);
    }

    private static void arrayReverse(byte[] array, int begin, int length) {
        int i = begin;
        int j = begin + length - 1;
        byte tmp;
        while (i < j) {
            tmp = array[i];
            array[i] = array[j];
            array[j] = tmp;
            ++i;
            --j;
        }
    }

    private static int getStrLength(byte[] array, int begin) {
        int length = 0;
        while (array[begin] != '\0') {
            ++length;
            ++begin;
        }
        return length;
    }

    public static byte[] hexToByte(String hexValue) {
        if (null == hexValue || hexValue.isEmpty()) {
            return new byte[0];
        }
        int bLen = hexValue.length() / 2;
        byte[] b = new byte[bLen];
        for (int i = 0; i < bLen; i++) {
            String subStr = hexValue.substring(i * 2, i * 2 + 2);
            b[i] = (byte) Integer.parseInt(subStr, 16);
        }

        return b;
    }

    public static byte[] sha256(String original)
            throws NoSuchAlgorithmException, UnsupportedEncodingException {
        return sha256(original.getBytes("UTF-8"));
    }

    public static byte[] sha256(byte[] original) {
        try {
            MessageDigest md;
            md = MessageDigest.getInstance("SHA-256");
            md.update(original);
            return md.digest();
        }
        catch (Exception e) {
            throw new BaseException(SDBError.SDB_INVALIDARG, "sha256 failed", e);
        }
    }

    public static void main(String[] args) {
        byte[] b = Helper.hexToByte(new String("7F01"));
        for (int i = 0; i < b.length; i++) {
            System.out.println(b[i]);
        }
    }
}
