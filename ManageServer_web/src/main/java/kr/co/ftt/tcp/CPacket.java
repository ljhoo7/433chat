package kr.co.ftt.tcp;

import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class CPacket {
    public byte[] buffer;
    public int position;
    public short protocol_id;

    public static CPacket create(short protocol_id) {
        CPacket packet = new CPacket();
        packet.set_protocol(protocol_id);
        return packet;
    }

    public CPacket(byte[] buffer) {
        this.buffer = buffer;
        this.position = 2;
    }

    public CPacket() {
        this.buffer = new byte[1024];
    }

    public short pop_protocol_id() {
        return pop_int16();
    }

    public void copy_to(CPacket target) {
        target.set_protocol(this.protocol_id);
        target.overwrite(this.buffer, this.position);
    }

    public void overwrite(byte[] source, int position) {
        System.arraycopy(source, 0, this.buffer,0, source.length);
        this.position = position;
    }

    public byte pop_byte() {
        byte data = this.buffer[this.position];
        this.position += 1;
        return data;
    }

    public short pop_int16() {
        ByteBuffer bb = ByteBuffer.allocate(2);
        bb.order(ByteOrder.LITTLE_ENDIAN);
        bb.put(this.buffer[this.position]);
        bb.put(this.buffer[this.position + 1]);
        short shortVal = bb.getShort(0);
        this.position += 2;
        return shortVal;
    }

    public short pop_int32() {
        ByteBuffer bb = ByteBuffer.allocate(4);
        bb.order(ByteOrder.LITTLE_ENDIAN);
        bb.put(this.buffer[this.position]);
        bb.put(this.buffer[this.position + 1]);
        bb.put(this.buffer[this.position + 2]);
        bb.put(this.buffer[this.position + 3]);
        int intVal = bb.getInt(0);
        this.position += 4;
        return (short) intVal;
    }

    public String pop_string() {
        short len = pop_int16();
        byte[] b = new byte[len];
        System.arraycopy(buffer, position, b, 0, len);
        String data = null;
		try {
			data = new String(b, "UTF-8");
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}
        this.position += len;
        return data;
    }

    public void set_protocol(short protocol_id){
        this.protocol_id = protocol_id;
        this.position = 2;
        push(protocol_id);
    }
    public void record_size()
    {
        short body_size = (short)(this.position - 2);
        ByteBuffer bb = ByteBuffer.allocate(2);
        bb.order(ByteOrder.LITTLE_ENDIAN);
        bb.putShort(body_size);
        System.arraycopy(bb.array(), 0, buffer, 0, 2);
    }
    public void push(short data){
        ByteBuffer bb = ByteBuffer.allocate(2);
        bb.order(ByteOrder.LITTLE_ENDIAN);
        bb.putShort(data);
        System.arraycopy(bb.array(), 0, buffer, position, 2);
        this.position += 2;
    }
    public void push(byte data){
        ByteBuffer bb = ByteBuffer.allocate(1);
        bb.order(ByteOrder.LITTLE_ENDIAN);
        bb.put(data);
        System.arraycopy(bb.array(), 0, buffer, position, 1);
        this.position += 1;
    }
    public void push(int data){
        ByteBuffer bb = ByteBuffer.allocate(4);
        bb.order(ByteOrder.LITTLE_ENDIAN);
        bb.putInt(data);
        System.arraycopy(bb.array(), 0, buffer, position, 4);
        this.position += 4;
    }
    public void push(String data) {
        byte[] text = null;
		try {
			text = data.getBytes("UTF-8");
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}
        short len = (short) text.length;
        push(len);
        System.arraycopy(text, 0, buffer, position, len);
        this.position += len;
    }
}