# RDA5807M_proxy
RDA5807M proxy for raspberry

The goal is to control the FM Receiver from the Raspberry.
Unfortunatly the Raspberry doesn't see all FM Receiver i2c busses (I don't know why).

The Arduino will be a bridge.

  -------------             --------------
 |             |           |              |            -----------
 | Fm Receiver |    i2c    | Arduino mini |    spi    |           |
 |   Grove     | <=======> |     3.3v     | <=======> | Raspberry |
 |             |           |    8 Mhz     |           |           |
  -------------            |              |            -----------
                            --------------

Project on going...

Link:
Grove - i2c FM Receiver info: http://wiki.seeed.cc/Grove-I2C_FM_Receiver/