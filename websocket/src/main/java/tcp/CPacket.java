package tcp;

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
        this.position = 0;
    }

    public CPacket() {
        this.buffer = new byte[1024];
    }

    public short pop_protocol_id() {
        return pop_short();
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

    public short pop_short() {
        ByteBuffer bb = ByteBuffer.allocate(2);
        bb.order(ByteOrder.LITTLE_ENDIAN);
        bb.put(this.buffer[this.position]);
        bb.put(this.buffer[this.position + 1]);
        short shortVal = bb.getShort(0);
        this.position += 2;
        return shortVal;
    }

    public short pop_int() {
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

    public String pop_string(int size) {
    	int i;
    	for (i=position; i<position+20; i++){
    		if (buffer[i]==0) break;
    	}
    	
        byte[] b = new byte[size];
        System.arraycopy(buffer, position, b, 0, i-position);
        String data = null;
		try {
			data = new String(b, "UTF-8");
		} catch (UnsupportedEncodingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
        this.position += size;
        return data;
    }

    public void set_protocol(short protocol_id){
        this.protocol_id = protocol_id;
        this.position = 0;
        push(protocol_id);
    }
    public void record_size(int offset, short size)
    {
        ByteBuffer bb = ByteBuffer.allocate(2);
        bb.order(ByteOrder.LITTLE_ENDIAN);
        bb.putShort(size);
        System.arraycopy(bb.array(), 0, buffer, offset, 2);
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
    
    public void push(byte[] data, int size){
        for (int i=0; i<size; i++){
        	push(data[i]);
        }
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
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
        short len = (short) text.length;
        System.arraycopy(text, 0, buffer, position, len);
        this.position += len;
        
        push((byte)0);
    }
}