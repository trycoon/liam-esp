LoRa (Long Range) is a radio modulation technique owned and developed by the Semtech company. They develop the popular [SX127\* radio modems](https://www.semtech.com/products/wireless-rf/lora-transceivers/SX1278) that offers a very long range (tens of kilometers) radio link, but with a very low bitrate (tens of kbps).

We use two LoRa modems, one in the mower and the other in the docking station. They are used for relaying request from the clients through the docking station to the mower, and for reporting mower status back to docking station and the clients. LoRa offers connections over a greater distance with higher reliability than ordinary WiFi (IEEE 802.11).

There are plenty of suitable LoRa modems out there, these are two common examples:

- ["Winext LM1278"](https://www.google.com/search?q=Winext+LM1278)
- ["AI-Thinker RA-02"](https://www.google.com/search?q=AI-Thinker+RA-02)

Remember to buy TWO of them, one for mower and one for docking station. Also make sure you select the [correct frequency for you country](https://www.thethingsnetwork.org/docs/lorawan/frequencies-by-country.html)!
If no antenna is included you have to buy TWO antennas for the matching frequency.

AND NEVER START THE MODULE WITHOUT THE ANTENNA CONNECTED (it will damage the modem)!

[SX1278 datasheets](sx1276_77_78_79.pdf)
