import comms_pb2 as Comms
from pwn import remote
from datetime import datetime as dt

KEY = b"c0189790ab65aa39"
server = remote("127.0.0.1", 5432)

# send configuration data
from_scooter = Comms.FromScooter()
from_scooter.state = Comms.State.ASLEEP
from_scooter.configuration.ble_version = 132
from_scooter.configuration.drv_version = 247
from_scooter.configuration.bms_version = 141
from_scooter.configuration.iot_version =   1
from_scooter.configuration.shfw_installed = True

server.sendline(KEY + from_scooter.SerializeToString())
server.recvuntil((b"OK", b"AUTH", b"ERR"))

for i in range(1440*28):
    time_of_day = i % 1440

    if time_of_day < 4 * 60:
        # online (4 hrs daily)
        from_scooter = Comms.FromScooter()
        from_scooter.state = Comms.State.ONLINE
        for j in range(60):
            # location update every 5s
            if j % 5 == 0 and j != 59:
                location_update = Comms.FromScooter()
                location_update.state = Comms.State.ONLINE
                location_update.timestamp = round(dt.now().timestamp() * 1000)
                location_update.asleep_data.location.latitude = 54.56
                location_update.asleep_data.location.latitude = -6.78

                server.sendline(KEY + location_update.SerializeToString())
                server.recvuntil((b"OK", b"AUTH", b"ERR"))

        server.sendline(KEY + from_scooter.SerializeToString())
        server.recvuntil((b"OK", b"AUTH", b"ERR"))
    elif time_of_day < 8 * 60:
        # charging (4 hrs daily)
        if i % 2 != 0:
            continue
        # charging update every 2 minutes
        from_scooter = Comms.FromScooter()
        from_scooter.state = Comms.State.CHARGING
        from_scooter.timestamp = round(dt.now().timestamp() * 1000)
        from_scooter.charging_data.battery_voltage = round(38.2 * 10)
        from_scooter.charging_data.battery_percentage = 67
        from_scooter.charging_data.amperage = round(28.4 * 10)
        from_scooter.charging_data.ecu_temperature = 37
        from_scooter.charging_data.battery_temperature = 41
        from_scooter.charging_data.remaining_mah = 6892

        server.sendline(KEY + from_scooter.SerializeToString())
        server.recvuntil((b"OK", b"AUTH", b"ERR"))
    else:
        # offline (rest of day)
        if i % 5 != 0:
            continue
        # location update every 5 minutes
        from_scooter = Comms.FromScooter()
        from_scooter.state = Comms.State.ASLEEP
        from_scooter.timestamp = round(dt.now().timestamp() * 1000)
        from_scooter.asleep_data.location.latitude = 54.56
        from_scooter.asleep_data.location.longitude = -6.78

        server.sendline(KEY + from_scooter.SerializeToString())
        server.recvuntil((b"OK", b"AUTH", b"ERR"))

server.close()
