package kr.co.ftt.ManageServer_receiver.util;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class ByteArrayToInt {
	public static int byteArrayToInt(byte[] bytes) {
		final int size = Integer.SIZE / 8;
		ByteBuffer buff = ByteBuffer.allocate(size);
		final byte[] newBytes = new byte[size];
		for (int i = 0; i < size; i++) {
			if (i + bytes.length < size) {
				newBytes[i] = (byte) 0x00;
			} else {
				newBytes[i] = bytes[i + bytes.length - size];
			}
		}
		buff = ByteBuffer.wrap(newBytes);
		buff.order(ByteOrder.LITTLE_ENDIAN); // Endian에 맞게 세팅
		return buff.getInt();
	}
}

