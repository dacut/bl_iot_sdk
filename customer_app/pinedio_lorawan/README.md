# LoRaWAN Firmware for PineDio Stack BL604

Based on [`sdk_app_lorawan`](../sdk_app_lorawan)

LoRaWAN Firmware with command-line interface, ported to BL602 from Apache Mynewt OS...

https://mynewt.apache.org/latest/tutorials/lora/lorawanapp.html

https://github.com/apache/mynewt-core/tree/master/apps/lora_app_shell

This firmware calls the LoRaWAN Driver...

-   [`lorawan`: BL602 LoRaWAN Driver](../../components/3rdparty/lorawan)

And the Semtech SX1262 Driver...

-   [`lora-sx1262`: Semtech SX1262 Driver](../../components/3rdparty/lora-sx1262)

Read the articles...

-   ["PineCone BL602 Talks LoRaWAN"](https://lupyuen.github.io/articles/lorawan)

-   ["LoRaWAN on PineDio Stack BL604 RISC-V Board"](https://lupyuen.github.io/articles/lorawan2)

-   ["The Things Network on PineDio Stack BL604 RISC-V Board"](https://lupyuen.github.io/articles/ttn)

-   ["Encode Sensor Data with CBOR on BL602"](https://lupyuen.github.io/articles/cbor)

-   ["Internal Temperature Sensor on BL602"](https://lupyuen.github.io/articles/tsen)

# LoRaWAN Commands

Run these commands to join the LoRaWAN Network and send data to the network...

```bash
#  Start LoRa background task
create_task

#  Init LoRaWAN driver
init_lorawan

#  Device EUI: Copy from ChirpStack: Applications -> app -> Device EUI
las_wr_dev_eui 0x4b:0xc1:0x5e:0xe7:0x37:0x7b:0xb1:0x5b

#  App EUI: Not needed for ChirpStack, set to default 0000000000000000
las_wr_app_eui 0x00:0x00:0x00:0x00:0x00:0x00:0x00:0x00

#  App Key: Copy from ChirpStack: Applications -> app -> Devices -> device_otaa_class_a -> Keys (OTAA) -> Application Key
las_wr_app_key 0xaa:0xff:0xad:0x5c:0x7e:0x87:0xf6:0x4d:0xe3:0xf0:0x87:0x32:0xfc:0x1d:0xd2:0x5d

#  Join LoRaWAN network, try 1 time
las_join 1

#  Open LoRaWAN port 2 (App Port)
las_app_port open 2

#  Send data to LoRaWAN port 2, 5 bytes, unconfirmed (0)
las_app_tx 2 5 0

#  Transmit the CBOR payload { "t": 1234, "l": 2345 } to port 2, unconfirmed (0)
las_app_tx_cbor 2 0 1234 2345

#  Transmit the CBOR payload { "t": 1234, "l": 2345 } to port 2, unconfirmed (0), 
#  for 10 times, with a 60 second interval. Assuming that the Internal Temperature Sensor
#  returns 12.34 degrees Celsius.
las_app_tx_tsen 2 0 2345 10 60
```

# LoRaWAN Commands for The Things Network

Run these commands to join The Things Network and send data to the network...

```bash
#  Start LoRa background task
create_task

#  Init LoRaWAN driver
init_lorawan

#  Copy the following values from The Things Network Console -> 
#  Applications -> (Your App) -> End Devices -> (Your Device)...

#  Device EUI: Copy from (Your Device) -> DevEUI
las_wr_dev_eui 0xAB:0xBA:0xDA:0xBA:0xAB:0xBA:0xDA:0xBA

#  App EUI: Copy from (Your Device) -> JoinEUI
las_wr_app_eui 0x00:0x00:0x00:0x00:0x00:0x00:0x00:0x00

#  App Key: Copy from (Your Device) -> AppKey
las_wr_app_key 0xAB:0xBA:0xDA:0xBA:0xAB:0xBA:0xDA:0xBA0xAB:0xBA:0xDA:0xBA:0xAB:0xBA:0xDA:0xBA

#  Join The Things Network, try 1 time
las_join 1

#  Open The Things Network port 2 (App Port)
las_app_port open 2

#  Send data to The Things Network port 2, 5 bytes, unconfirmed (0)
las_app_tx 2 5 0

#  Transmit the CBOR payload { "t": 1234, "l": 2345 } to port 2, unconfirmed (0)
las_app_tx_cbor 2 0 1234 2345

#  Transmit the CBOR payload { "t": 1234, "l": 2345 } to port 2, unconfirmed (0), 
#  for 10 times, with a 60 second interval. Assuming that the Internal Temperature Sensor
#  returns 12.34 degrees Celsius.
las_app_tx_tsen 2 0 2345 10 60
```

# Message Integrity Code Errors

To search for Message Integrity Code errors in LoRaWAN Packets received by WisGate, SSH to WisGate and search for...

```bash
# grep MIC /var/log/syslog

Apr 28 04:02:05 rak-gateway 
chirpstack-application-server[568]: 
time="2021-04-28T04:02:05+01:00" 
level=error 
msg="invalid MIC" 
dev_eui=4bc15ee7377bb15b 
type=DATA_UP_MIC

Apr 28 04:02:05 rak-gateway 
chirpstack-network-server[1378]: 
time="2021-04-28T04:02:05+01:00" 
level=error 
msg="uplink: processing uplink frame error"
ctx_id=0ccd1478-3b79-4ded-9e26-a28e4c143edc 
error="get device-session error: invalid MIC"
```

The error above occurs when we replay a repeated Join Network Request to our LoRaWAN Gateway (with same Nonce, same Message Integrity Code).

This replay also logs a Nonce Error in WisGate...

```bash
# grep nonce /var/log/syslog

Apr 28 04:02:41 rak-gateway chirpstack-application-server[568]:
time="2021-04-28T04:02:41+01:00" 
level=error 
msg="validate dev-nonce error" 
dev_eui=4bc15ee7377bb15b 
type=OTAA

Apr 28 04:02:41 rak-gateway chirpstack-network-server[1378]:
time="2021-04-28T04:02:41+01:00" 
level=error 
msg="uplink: processing uplink frame error" ctx_id=01ae296e-8ce1-449a-83cc-fb0771059d89 
error="validate dev-nonce error: object already exists"
```

Because the Nonce should not be reused.

# LoRa Packet Forwarder for WisGate

Config file is at...

```text
/opt/ttn-gateway/packet_forwarder/lora_pkt_fwd/global_conf.json
```

Restart packet forwarder...

```bash
sudo bash
cd /opt/ttn-gateway/packet_forwarder/lora_pkt_fwd
pkill -9 lora_pkt_fwd
./lora_pkt_fwd
```

# Output Log

Captured on PineDio Stack BL604, talking to WisGate D4H LoRaWAN Gateway with ChirpStack...

```text
▒Starting bl602 now....
Booting BL602 Chip...
██████╗ ██╗      ██████╗  ██████╗ ██████╗
██╔══██╗██║     ██╔════╝ ██╔═████╗╚════██╗
██████╔╝██║     ███████╗ ██║██╔██║ █████╔╝
██╔══██╗██║     ██╔═══██╗████╔╝██║██╔═══╝
██████╔╝███████╗╚██████╔╝╚██████╔╝███████╗
╚═════╝ ╚══════╝ ╚═════╝  ╚═════╝ ╚══════╝


------------------------------------------------------------
RISC-V Core Feature:RV32-ACFIMX
Build Version: release_bl_iot_sdk_1.6.11-1-g66bb28da-dirty
Build Date: Sep 10 2021
Build Time: 11:57:34
-----------------------------------------------------------

blog init set power on level 2, 2, 2.
[IRQ] Clearing and Disable all the pending IRQ...
[OS] Starting aos_loop_proc task...
[OS] Starting OS Scheduler...
=== 32 task inited
====== bloop dump ======
  bitmap_evt 0
  bitmap_msg 0
--->>> timer list:
  32 task:
    task[31] : SYS [built-in]
      evt handler 0x2301c07c, msg handler 0x2301c046, trigged cnt 0, bitmap asyn c 0 sync 0, time consumed 0us acc 0ms, max 0us
    task[30] : empty
    task[29] : empty
    task[28] : empty
    task[27] : empty
    task[26] : empty
    task[25] : empty
    task[24] : empty
    task[23] : empty
    task[22] : empty
    task[21] : empty
    task[20] : empty
    task[19] : empty
    task[18] : empty
    task[17] : empty
    task[16] : empty
    task[15] : empty
    task[14] : empty
    task[13] : empty
    task[12] : empty
    task[11] : empty
    task[10] : empty
    task[09] : empty
    task[08] : empty
    task[07] : empty
    task[06] : empty
    task[05] : empty
    task[04] : empty
    task[03] : empty
    task[02] : empty
    task[01] : empty
    task[00] : empty
Init CLI with event Driven

# create_task

# init_lorawan
Set Flash CS pin 14 to high
Set ST7789 CS pin 20 to high
Set SX1262 CS pin 15 to high
Set Debug CS pin 5 to high
lora_node_init
pbuf_queue_init
pbuf_queue_init
pbuf_queue_init
SX126xReset
SX126xIoInit
Swap MISO and MOSI
SX126X interrupt init
SX126X register handler: GPIO 19
SX126xWakeup
SX126xSetTxParams: power=22, rampTime=7
SX126xGetDeviceId: SX1262
SX126xSetPaConfig: paDutyCycle=4, hpMax=7, deviceSel=0, paLut=1
RadioSetModem
RadioSetPublicNetwork: public syncword=3444
RadioSleep

# las_wr_dev_eui 0x4b:0xc1:0x5e:0xe7:0x37:0x7b:0xb1:0x5b

# las_wr_app_eui 0x00:0x00:0x00:0x00:0x00:0x00:0x00:0x00

# las_wr_app_key 0xaa:0xff:0xad:0x5c:0x7e:0x87:0xf6:0x4d:0xe3:0xf0:0x87:0x32:0xf c:0x1d:0xd2:0x5d

# las_join 1
lora_node_join: joined=8
lora_node_join: joining network
Attempting to join...

# lora_mac_join_event
LoRaMacMlmeRequest
Send
RadioSetModem
SX126xWakeup
ScheduleTx
CalculateBackOff
RegionNextChannel
RegionAS923NextChannel
RegionAS923NextChannel: channel=1
RegionComputeRxWindowParameters
RegionComputeRxWindowParameters
lora_mac_rx_disable
TODO: Radio.RxDisable
SendFrameOnChannel: channel=1
RegionTxConfig
RegionAS923TxConfig
RadioSetChannel: freq=923400000
RadioSetTxConfig: modem=1, power=13, fdev=0, bandwidth=0, datarate=10, coderate= 1, preambleLen=8, fixLen=0, crcOn=1, freqHopOn=0, hopPeriod=0, iqInverted=0, tim eout=3000
RadioSetTxConfig: SpreadingFactor=10, Bandwidth=4, CodingRate=1, LowDatarateOpti mize=0, PreambleLength=8, HeaderType=0, PayloadLength=255, CrcMode=1, InvertIQ=0
RadioStandby
RadioSetModem
SX126xSetRfTxPower
SX126xSetTxParams: power=13, rampTime=7
SX126xGetDeviceId: SX1262
SX126xSetPaConfig: paDutyCycle=4, hpMax=7, deviceSel=0, paLut=1
SendFrameOnChannel: channel=1, datarate=2, txpower=0, maxeirp=16, antennagain=2
SendFrameOnChannel: txi is null, skipping log
RadioSend: size=23
00 00 00 00 00 00 00 00 00 5b b1 7b 37 e7 5e c1 4b 8b 22 2e 53 d4 79
RadioSend: PreambleLength=8, HeaderType=0, PayloadLength=23, CrcMode=1, InvertIQ =0
lora_mac_join_event: OK
RadioOnDioIrq
RadioIrqProcess
SX126xReadCommand
IRQ_TX_DONE
OnRadioTxDone
RadioSleep

# OnRxWindow1TimerEvent
RadioSetChannel: freq=923400000
SX126xWakeup
RadioSetRxConfig
RadioStandby
RadioSetModem
RadioSetRxConfig done
RadioRx
RadioOnDioIrq
RadioIrqProcess
SX126xReadCommand
IRQ_PREAMBLE_DETECTED
RadioOnDioIrq
RadioIrqProcess
SX126xReadCommand
IRQ_HEADER_VALID
RadioOnDioIrq
RadioIrqProcess
SX126xReadCommand
IRQ_RX_DONE
SX126xReadCommand
SX126xReadCommand
OnRadioRxDone
loramac_process_radio_rx
RadioSleep
lora_mac_rx_win2_stop

# las_app_port open 2
Opened app port 2

# las_app_tx 2 5 0
lwip_init
-------------------->>>>>>>> LWIP tcp_port 51766
lora_node_mcps_request
pbuf_queue_put
Packet sent on port 2

# lora_mac_proc_tx_q_event
lora_mac_proc_tx_q_event: send from txq
pbuf_queue_get
Send
ScheduleTx
CalculateBackOff
RegionNextChannel
RegionAS923NextChannel
RegionAS923NextChannel: channel=1
RegionComputeRxWindowParameters
RegionComputeRxWindowParameters
lora_mac_rx_disable
TODO: Radio.RxDisable
SendFrameOnChannel: channel=1
RegionTxConfig
RegionAS923TxConfig
RadioSetChannel: freq=923400000
SX126xWakeup
RadioSetTxConfig: modem=1, power=13, fdev=0, bandwidth=0, datarate=10, coderate= 1, preambleLen=8, fixLen=0, crcOn=1, freqHopOn=0, hopPeriod=0, iqInverted=0, tim eout=3000
RadioSetTxConfig: SpreadingFactor=10, Bandwidth=4, CodingRate=1, LowDatarateOpti mize=0, PreambleLength=8, HeaderType=0, PayloadLength=64, CrcMode=1, InvertIQ=0
RadioStandby
RadioSetModem
SX126xSetRfTxPower
SX126xSetTxParams: power=13, rampTime=7
SX126xGetDeviceId: SX1262
SX126xSetPaConfig: paDutyCycle=4, hpMax=7, deviceSel=0, paLut=1
SendFrameOnChannel: channel=1, datarate=2, txpower=0, maxeirp=16, antennagain=2
RadioSend: size=18
40 53 f4 bd 01 00 00 00 02 19 55 bf ad 10 69 90 ea a3
RadioSend: PreambleLength=8, HeaderType=0, PayloadLength=18, CrcMode=1, InvertIQ =0
RadioOnDioIrq
RadioIrqProcess
SX126xReadCommand
IRQ_TX_DONE
OnRadioTxDone
RadioSleep
OnRxWindow1TimerEvent
RadioSetChannel: freq=923400000
SX126xWakeup
RadioSetRxConfig
RadioStandby
RadioSetModem
RadioSetRxConfig done
RadioRx
RadioOnDioIrq
RadioIrqProcess
SX126xReadCommand
IRQ_RX_TX_TIMEOUT
OnRadioRxTimeout
RadioSleep
OnRxWindow2TimerEvent
RadioSetChannel: freq=923200000
SX126xWakeup
RadioSetRxConfig
RadioStandby
RadioSetModem
RadioSetRxConfig done
RadioRx
RadioOnDioIrq
RadioIrqProcess
SX126xReadCommand
IRQ_RX_TX_TIMEOUT
OnRadioRxTimeout
RadioSleep
pbuf_queue_put
lora_node_chk_txq
pbuf_queue_get
Txd on port 2 type=unconf status=0 len=5
        dr:2
        txpower (dbm):0
        tries:1
        ack_rxd:0
        tx_time_on_air:330
        uplink_cntr:0
        uplink_chan:1
pbuf_queue_get
lora_mac_proc_tx_q_event

#
# las_app_tx 2 5 0
lora_node_mcps_request
pbuf_queue_put
Packet sent on port 2

# lora_mac_proc_tx_q_event
lora_mac_proc_tx_q_event: send from txq
pbuf_queue_get
Send
ScheduleTx
CalculateBackOff
RegionNextChannel
RegionAS923NextChannel
RegionAS923NextChannel: channel=0
RegionComputeRxWindowParameters
RegionComputeRxWindowParameters
lora_mac_rx_disable
TODO: Radio.RxDisable
SendFrameOnChannel: channel=0
RegionTxConfig
RegionAS923TxConfig
RadioSetChannel: freq=923200000
SX126xWakeup
RadioSetTxConfig: modem=1, power=13, fdev=0, bandwidth=0, datarate=10, coderate= 1, preambleLen=8, fixLen=0, crcOn=1, freqHopOn=0, hopPeriod=0, iqInverted=0, tim eout=3000
RadioSetTxConfig: SpreadingFactor=10, Bandwidth=4, CodingRate=1, LowDatarateOpti mize=0, PreambleLength=8, HeaderType=0, PayloadLength=64, CrcMode=1, InvertIQ=0
RadioStandby
RadioSetModem
SX126xSetRfTxPower
SX126xSetTxParams: power=13, rampTime=7
SX126xGetDeviceId: SX1262
SX126xSetPaConfig: paDutyCycle=4, hpMax=7, deviceSel=0, paLut=1
SendFrameOnChannel: channel=0, datarae=2, txpower=0, maxeirp=16, antennagain=2
RadioSend: size=18
40 53 f4 bd 01 00 01 00 02 f4 d9 81 d4 03 02 03 83 af
RadioSend: PreambleLength=8, HeaderType=0, PayloadLength=18, CrcMode=1, InvertIQ =0
RadioOnDioIrq
RadioIrqProcess
SX126xReadCommand
IRQ_TX_DONE
OnRadioTxDone
RadioSleep
OnRxWindow1TimerEvent
RadioSetChannel: freq=923200000
SX126xWakeup
RadioSetRxConfig
RadioStandby
RadioSetModem
RadioSetRxConfig done
RadioRx
RadioOnDioIrq
RadioIrqProcess
SX126xReadCommand
IRQ_RX_TX_TIMEOUT
OnRadioRxTimeout
RadioSleep
OnRxWindow2TimerEvent
RadioSetChannel: freq=923200000
SX126xWakeup
RadioSetRxConfig
RadioStandby
RadioSetModem
RadioSetRxConfig done
RadioRx
RadioOnDioIrq
RadioIrqProcess
SX126xReadCommand
IRQ_RX_TX_TIMEOUT
OnRadioRxTimeout
RadioSleep
pbuf_queue_put
lora_node_chk_txq
pbuf_queue_get
Txd on port 2 type=unconf status=0 len=5
        dr:2
        tpower (dbm):0
        tries:1
        ack_rxd:0
        tx_time_on_air:330
        uplink_cntr:1
        uplink_chan:0
pbuf_queue_get
lora_mac_proc_tx_q_event

#
```

Output Log for `las_app_tx_tsen` (transmit internal temperature sensor)...

```text
# las_app_tx_tsen 2 0 4000 10 60
offset = 2175
temperature = 44.885849 Celsius
Encode CBOR: { t: 4488, l: 4000 }
CBOR Output: 11 bytes
  0xa2 0x61 0x74 0x19 0x11 0x88 0x61 0x6c 0x19 0x0f 0xa0
lwip_init
-------------------->>>>>>>> LWIP tcp_port 49173
lora_node_mcps_request
pbuf_queue_put
Packet sent on port 2
lora_mac_proc_tx_q_event
lora_mac_proc_tx_q_event: send from txq
pbuf_queue_get
Send
ScheduleTx
CalculateBackOff
RegionNextChannel
RegionAS923NextChannel
RegionAS923NextChannel: channel=2
RegionComputeRxWindowParameters
RegionComputeRxWindowParameters
lora_mac_rx_disable
TODO: Radio.RxDisable
SendFrameOnChannel: channel=2
RegionTxConfig
RegionAS923TxConfig
RadioSetChannel: freq=922200000
SX126xWakeup
RadioSetTxConfig: modem=1, power=13, fdev=0, bandwidth=0, datarate=10, coderate=1, preambleLen=8, fixLen=0, crcOn=1, freqHopOn=0, hopPeriod=0, iqInverted=0, timeout=3000
RadioSetTxConfig: SpreadingFactor=10, Bandwidth=4, CodingRate=1, LowDatarateOptimize=0, PreambleLength=8, HeaderType=0, PayloadLength=64, CrcMode=1, InvertIQ=0
RadioStandby
RadioSetModem
SX126xSetRfTxPower
SX126xSetTxParams: power=13, rampTime=7
SX126xGetDeviceId: SX1262
SX126xSetPaConfig: paDutyCycle=4, hpMax=7, deviceSel=0, paLut=1
SendFrameOnChannel: channel=2, datarate=2, txpower=0, maxeirp=16, antennagain=2
RadioSend: size=24
40 1a 10 0d 26 00 00 00 02 21 c5 80 e9 05 1d eb b6 1c 53 59 20 69 01 9d
RadioSend: PreambleLength=8, HeaderType=0, PayloadLength=24, CrcMode=1, InvertIQ=0
RadioOnDioIrq
RadioIrqProcess
SX126xReadCommand
IRQ_TX_DONE
OnRadioTxDone
RadioSleep
OnRxWindow1TimerEvent
RadioSetChannel: freq=922200000
SX126xWakeup
RadioSetRxConfig
RadioStandby
RadioSetModem
RadioSetRxConfig done
RadioRx
RadioOnDioIrq
RadioIrqProcess
SX126xReadCommand
IRQ_RX_TX_TIMEOUT
OnRadioRxTimeout
RadioSleep
OnRxWindow2TimerEvent
RadioSetChannel: freq=923200000
SX126xWakeup
RadioSetRxConfig
RadioStandby
RadioSetModem
RadioSetRxConfig done
RadioRx
RadioOnDioIrq
RadioIrqProcess
SX126xReadCommand
IRQ_RX_TX_TIMEOUT
OnRadioRxTimeout
RadioSleep
pbuf_queue_put
lora_node_chk_txq
pbuf_queue_get
Txd on port 2 type=unconf status=0 len=11
        dr:2
        txpower (dbm):0
        tries:1
        ack_rxd:0
        tx_time_on_air:371
        uplink_cntr:0
        uplink_chan:2
pbuf_queue_get
lora_mac_proc_tx_q_event
offset = 2175
temperature = 47.207531 Celsius
Encode CBOR: { t: 4720, l: 4000 }
CBOR Output: 11 bytes
  0xa2 0x61 0x74 0x19 0x12 0x70 0x61 0x6c 0x19 0x0f 0xa0
lora_node_mcps_request
pbuf_queue_put
Packet sent on port 2
lora_mac_proc_tx_q_event
lora_mac_proc_tx_q_event: send from txq
pbuf_queue_get
Send
ScheduleTx
CalculateBackOff
RegionNextChannel
RegionAS923NextChannel
egionAS923NextChannel: channel=0
RegionComputeRxWindowParameters
RegionComputeRxWindowParameters
lora_mac_rx_disable
TODO: Radio.RxDisable
SendFrameOnChannel: channel=0
RegionTxConfig
RegionAS923TxConfig
RadioSetChannel: freq=923200000
SX126xWakeup
RadioSetTxConfig: modem=1, power=13, fdev=0, bandwidth=0, datarate=10, coderate=1, preambleLen=8, fixLen=0, crcOn=1, freqHopOn=0, hopPeriod=0, iqInverted=0, timeout=3000
RadioSetTxConfig: SpreadingFactor=10, Bandwidth=4, CodingRate=1, LowDatarateOptimize=0, PreambleLength=8, HeaderType=0, PayloadLength=64, CrcMode=1, InvertIQ=0
RadioStandby
RadioSetModem
SX126xSetRfTxPower
SX126xSetTxParams: power=13, rampTime=7
SX126xGetDeviceId: SX1262
SX126xSetPaConfig: paDutyCycle=4, hpMax=7, deviceSel=0, paLut=1
SendFrameOnChannel: channel=0, datarate=2, txpower=0, maxeirp=16, antennagain=2
RadioSend: size=24
40 1a 10 0d 26 00 01 00 02 a2 a3 b4 06 f0 c2 69 6e be 36 30 bf 9c 29 e3
RadioSend: PreambleLength=8, HeaderType=0, PayloadLength=24, CrcMode=1, InvertIQ=0
RadioOnDioIrq
RadioIrqProcess
SX126xReadCommand
IRQ_TX_DONE
OnRadioTxDone
RadioSleep
OnRxWindow1TimerEvent
RadioSetChannel: freq=923200000
SX126xWakeup
RadioSetRxConfig
RadioStandby
RadioSetModem
RadioSetRxConfig done
RadioRx
RadioOnDioIrq
RadioIrqProcess
SX126xReadCommand
IRQ_PREAMBLE_DETECTED
RadioOnDioIrq
RadioIrqProcess
SX126xReadCommand
IRQ_HEADER_VALID
RadioOnDioIrq
RadioIrqProcess
SX126xReadCommand
IRQ_RX_DONE
SX126xReadCommand
SX126xReadCommand
OnRadioRxDone
lora_mac_process_radio_rx
RadioSleep
lora_mac_rx_win2_stop
lora_mac_rtx_timer_stop
pbuf_queue_put
lora_node_chk_txq
pbuf_queue_get
Txd on port 2 type=unconf status=0 len=11
        dr:2
        txpower (dbm):0
        tries:1
        ack_rxd:0
        tx_time_on_air:371
        uplink_cntr:1
        uplink_chan:0
pbuf_queue_get
lora_mac_proc_tx_q_event
lora_mac_proc_tx_q_event: send empty msg
Send
ScheduleTx
CalculateBackOff
RegionNextChannel
RegionAS923NextChannel
RegionAS923NextChannel: channel=1
RegionComputeRxWindowParameters
RegionComputeRxWindowParameters
lora_mac_rx_disable
TODO: Radio.RxDisable
SendFrameOnChannel: channel=1
RegionTxConfig
RegionAS923TxConfig
RadioSetChannel: freq=923400000
SX126xWakeup
RadioSetTxConfig: modem=1, power=13, fdev=0, bandwidth=0, datarate=10, coderate=1, preambleLen=8, fixLen=0, crcOn=1, freqHopOn=0, hopPeriod=0, iqInverted=0, timeout=3000
RadioSetTxConfig: SpreadingFactor=10, Bandwidth=4, CodingRate=1, LowDatarateOptimize=0, PreambleLength=8, HeaderType=0, PayloadLength=64, CrcMode=1, InvertIQ=0
RadioStandby
RadioSetModem
SX126xSetRfTxPower
SX126xSetTxParams: power=13, rampTime=7
SX126xGetDeviceId: SX1262
SX126xSetPaConfig: paDutyCycle=4, hpMax=7, deviceSel=0, paLut=1
SendFrameOnChannel: channel=1, datarate=2, txpower=0, maxeirp=16, antennagain=2
RadioSend: size=15
40 1a 10 0d 26 00 02 00 00 51 cd 88 53 99 7f
RadioSend: PreambleLength=8, HeaderType=0, PayloadLength=15, CrcMode=1, InvertIQ=0
RadioOnDioIrq
RadioIrqProcess
SX126xReadCommand
IRQ_TX_DONE
OnRadioTxDone
RadioSleep
OnRxWindow1TimerEvent
RadioSetChannel: freq=923400000
SX126xWakeup
RadioSetRxConfig
RadioStandby
RadioSetModem
RadioSetRxConfig done
RadioRx
RadioOnDioIrq
RadioIrqProcess
SX126xReadCommand
IRQ_RX_TX_TIMEOUT
OnRadioRxTimeout
RadioSleep
OnRxWindow2TimerEvent
RadioSetChannel: freq=923200000
SX126xWakeup
RadioSetRxConfig
RadioStandby
RadioSetModem
RadioSetRxConfig done
RadioRx
RadioOnDioIrq
RadioIrqProcess
SX126xReadCommand
IRQ_RX_TX_TIMEOUT
OnRadioRxTimeout
RadioSleep
lora_node_chk_txq
lora_mac_proc_tx_q_event
offset = 2175
temperature = 48.755322 Celsius
Encode CBOR: { t: 4875, l: 4000 }
CBOR Output: 11 bytes
  0xa2 0x61 0x74 0x19 0x13 0x0b 0x61 0x6c 0x19 0x0f 0xa0
lora_node_mcps_request
pbuf_queue_put
Packet sent on port 2
lora_mac_proc_tx_q_event
lora_mac_proc_tx_q_event: send from txq
pbuf_queue_get
Send
ScheduleTx
CalculateBackOff
RegionNextChannel
RegionAS923NextChannel
RegionAS923NextChannel: channel=4
RegionComputeRxWindowParameters
RegionComputeRxWindowParameters
lora_mac_rx_disable
TODO: Radio.RxDisable
SendFrameOnChannel: channel=4
RegionTxConfig
RegionAS923TxConfig
RadioSetChannel: freq=922600000
SX126xWakeup
RadioSetTxConfig: modem=1, power=13, fdev=0, bandwidth=0, datarate=10, coderate=1, preambleLen=8, fixLen=0, crcOn=1, freqHopOn=0, hopPeriod=0, iqInverted=0, timeout=3000
RadioSetTxConfig: SpreadingFactor=10, Bandwidth=4, CodingRate=1, LowDatarateOptimize=0, PreambleLength=8, HeaderType=0, PayloadLength=64, CrcMode=1, InvertIQ=0
RadioStandby
RadioSetModem
SX126xSetRfTxPower
SX126xSetTxParams: power=13, rampTime=7
SX126xGetDeviceId: SX1262
SX126xSetPaConfig: paDutyCycle=4, hpMax=7, deviceSel=0, paLut=1
SendFrameOnChannel: channel=4, datarate=2, txpower=0, maxeirp=16, antennagain=2
RadioSend: size=24
40 1a 10 0d 26 00 03 00 02 68 c0 6f fd 81 d0 e4 9a 6f b3 10 5d 5b 6c 4a
RadioSend: PreambleLength=8, HeaderType=0, PayloadLength=24, CrcMode=1, InvertIQ=0
RadioOnDioIrq
RadioIrqProcess
SX126xReadCommand
IRQ_TX_DONE
OnRadioTxDone
RadioSleep
OnRxWindow1TimerEvent
RadioSetChannel: freq=922600000
SX126xWakeup
RadioSetRxConfig
RadioStandby
RadioSetModem
RadioSetRxConfig done
RadioRx
RadioOnDioIrq
RadioIrqProcess
SX126xReadCommand
IRQ_RX_TX_TIMEOUT
OnRadioRxTimeout
RadioSleep
OnRxWindow2TimerEvent
RadioSetChannel: freq=923200000
SX126xWakeup
RadioSetRxConfig
RadioStandby
RadioSetModem
RadioSetRxConfig done
RadioRx
RadioOnDioIrq
RadioIrqProcess
SX126xReadCommand
IRQ_RX_TX_TIMEOUT
OnRadioRxTimeout
RadioSleep
pbuf_queue_put
lora_node_chk_txq
pbuf_queue_get
Txd on port 2 type=unconf status=0 len=11
        dr:2
        txpower (dbm):0
        tries:1
        ack_rxd:0
        tx_time_on_air:371
        uplink_cntr:3
        uplink_chan:4
pbuf_queue_get
lora_mac_proc_tx_q_event
```