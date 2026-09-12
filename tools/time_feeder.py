from serial import Serial
from serial.tools import list_ports
from time import sleep, time

BAUD_RATE = 115200

def find_port() -> str:
    ports = list_ports.comports()
    if not ports: return None
    for port in ports:
        description = (port.description or '').lower()
        manufacturer = (port.manufacturer or '').lower()
        info = f'{description} {manufacturer}'
        if any(x in info for x in ['cp210', 'ch340', 'ch341', 'esp32', 'usb serial', 'usb-serial']):
            return port.device
    return None

def sync_time() -> None:
    try:
        serial_port = Serial(find_port(), BAUD_RATE, timeout=2)
        sleep(2)
        
        current_epoch = int(time())
        message = f"[Time_Manager]{current_epoch}\n"
        serial_port.write(message.encode('utf-8'))

        print("Menunggu konfirmasi dari ESP32...")
        start_wait = time()
        
        while (time() - start_wait) < 3:
            if serial_port.in_waiting > 0:
                response = serial_port.readline().decode('utf-8', errors='ignore').strip()
                
                if response.startswith("[Time_Manager] Sinkronisasi via Serial sukses! epoch:"):
                    print(response)
                    ack_epoch = response.split(':')[1]
                    
                    if int(ack_epoch) == current_epoch:
                        print(f"Validasi Sukses!")
                    else:
                        print("Validasi Gagal!")
        
        serial_port.close()
    except Exception as e:
        print(f"Gagal koneksi: {e}")

if __name__ == '__main__':
    sync_time()