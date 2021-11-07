Modbus & TCP Data Fetcher extend modification
==========
Copyright (c) 2019-2021 Yaroslav Shkliar
Copyright (c) 2018-2020 ILIT.RU, Research IT Laboratory 

http://www.cleenair.ru

This program is free software; you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2 of the License, or (at your option)
any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
details.


Modbus & TCP Data Fetcher?
----------------

Modbus & TCP Data Fetcher is a free Qt-based implementation of application wich oriented on ecological acquisition equipments.
This program tested with MOXA 5150, 5450 series devices Nport interaction in RealCom and TCP Server modes. Data Fetcher allow to collect measurements to postgresql DB and to edit it in interactive mode.


Supported equipments
--------------------

Gas analyzers total:
Optek JSC - RF;
League OJSC - RF;
Gamma ET - RF;
FEBA (JSC CIIK) - RF;
Serinus models by Ecotech Pty Ltd - Australia;
Grimm GmbH & Co Kr - Germany;
Turnkey tools - UK;
TSI Corp. - USA.

Meteocomplexes:

Davis Instruments - USA;
Lufft GmbH - Germany;
Vaisala Corp. - Finland;

IVTM - Exciss JSC RF.

Power supplies (all kinds of vendors supporting SNMP management protocol).

Security and fire alarm (JSC Elesta - full integration into the system or any without integration).

Air conditioning systems (any, as well as with WiFi support - direct integration).


Requirements
------------

* Qt5.12
* GCC
* libsnmp-dev 5.7.3
* gSoap 2.8
* libtelnet-dev

Building
--------

See INSTALL for information on how to build Modbus Data Fetcher.

Contact
-------

Any suggestions are welcome. Simply send a mail to
mail@ilit.ru
