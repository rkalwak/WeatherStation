# Stacja pogodowa

Z Arduino i zaraz potem ESP32 bawię się już ponad dwa lata. W sumie to zacząłem bardziej z [NanoFramework](https://www.nanoframework.net) i tam portowałem kilka bibliotek, bo C++ to jest dla mnie epoka kamienia łupanego :P Padł pomysł, żeby zrobić sobie pogodynkę na bazie ESP32 zaprogramowaną w C# przez NanoFramework. 

W miedzyczasie poznałem Suplę i zacząłem od rolet na SRW-01 a potem na zaprogramowaniu ESP przez GUI-Generic i się zaczęło... 

Doszedłem do momentu że dopisałem sobie "Kanał ogólnego przeznaczenia", umiem to zbudować ale ze względu na absolutny brak dokumentacji nie umiem tego uruchomić w Dokerze i się poddałem... 

Chciałem też napisać komunikację z Supla w C# ale z braku czasu odpuściłem.  Prędzej czy później to zrobię... 

Poczytałem trochę co będzie potrzebne, kupiłem niekompletna stacje Sainlogic WS3500 na A****** i tak powstało to cudo, które niedlugo zawiśnie na płocie :D

![stacja](/images/stacja.png)

## Funkcjonalności:
* 2x temperatura,
* wilgotność,
* ciśnienie atmosferyczne,
* siła i kierunek wiatru,
* ilość opadów,
* pomiar napięcia baterii,

![aplikacja](/images/aplikacja.png)

## Części:
* niekompletna stacja Sainlogic WS3500 - bez tabletu i zasilacza -> nie można jej skonfigurować, ale nieuszkodzona, wyrwana za 100zł :D
* ESP32 DevKit
* HDC1080 - czujnik wilgotności i temperatury
* MS5611 - czujnik ciśnienia
* DS18B20 - czujnik temperatury przy gruncie
* LTR390 - czujnik UV i natężenia światła wbudowany w stację, użyty jako ten drugi
* AS5600 - enkoder magnetyczny, zastąpił czujnik kierunku wiatru w stacji
* czujnik Halla - zastąpil czujnik siły wiatru w stacji , ten od czujnika opadów został
* 2 baterie 18650 LiPo i koszyk
* lx-lifc1-n - Moduł BMS z ładowarką do akumulatorów Li-ion 2S z USB typu C i obsługą QC, możliwość ładowania z USB albo panelu.
* przetwornica step-down
* panel fotowoltaiczny dający 7V i ok. 1W wbudowany w stację


## Schemat i sposób działania
![fritzing](/images/fritzing.png)

ESP32 jest wyciągniete do zewnętrznej puszki wraz z zasilaniem, ze względu na rozmiar stacji, po prostu się w niej nie mieści. Użyłem 12 żyłowego, 2 metrowego przewodu telefonicznego do połaczenia płytki w stacji z tą w puszce.

Wykorzystałem panel fotowoltaiczny z kupionej stacji do ładowania akumulatorków, jednak jest za słaby aby w pełni naładować 2 akumulatorki i co najwyżej wydłuża czas pracy na bateriach, który bez panelu wynosi ponad dobę pracy non stop.

## Problemy

### Brak schematów do stacji Sainlogic WS3500 i opisów na płytkach

Metodą prób i błędów wymyśliłem jak wykorzystać niektóre wbudowane czujniki.

LTR390 po wyjęciu i zobaczeniu oznaczeń pinów podpiąłem do ESP i puściłem skaner I2C a potem po adresie znalazłem czujnik, po dobraniu biblioteki zadziałał :D 

Niestety ten sam manewr nie zadziałał z czujnikiem temperatury i wilgotności. Mimo, że wygląda jak AHT XX i ma właściwy adres I2C nie działa z żadną biblioteką do tych czujników.

Czujnik Halla dla opadów zadziałał. Doświadczalnie trzeba było dobrać jego pojemność.

Czujnik Halla dla siły wiatru najpierw działał a potem przestał... Kupiłem inny i działa, ale obserwując dane w Supli, muszę poprawić obliczanie prędkości wiatru.

Po kilku godzinach czytania okazało się, że kierunek wiatru jest zrealizowany przez dwa czujniki Halla i coś jeszcze, ale nie rozkminiłem tego... Kupiłem za to enkoder magnetyczny, który podaję kąt w stopniach - idealnie.

### Zasilanie

Pierwsza opcja: 1 akumulatorek 18650 i LDO na 3.3V, owszem ESP się uruchomi, ale z Wifi już się nie połączy przy obciążeniu wszystkimi czujnikami. Użyłem AMS1117 jako LDO. 

Druga opcja: 2 akumulatorki 18650 i przetwornica StepDown ustawiona na 5V, podane na VIN pin. Działa, ale chyba marnuje możliwości akumulatorków. Dodatkowo dwa akumulatorki to za dużo aby naładować z małego panela dającego max 7V, więc chyba skończy się to większym panelem :D

Trzecia opcja: zasilanie z USB - nie mam póki co dobrego miejsca, żeby stacja wisiała na rozsądnej i dostępnej wysokości i był bezpieczny i wygodny dostęp do prądu.

Czwarta opcja: czytałem, że ESP32 DevKit może przyjąć do 12V na VIN pin, podłączył bym bezpośrednio baterie, ale jakoś się nie odważyłem jeszcze, ktoś próbował?

### Dokładność pomiarów

ADC w ESP32 jest trochę słabe, pomiar napięcia akumulatorków jest "mniej więcej", wahania rzędu 0.3V.

HDC1080 się bardzo grzeje w obudowie radiacyjnej i generalnie w południe pokazuje głupoty w stosunku do DS18B20 wiszącego luźno w cieniu, aczkolwiek wilgotność pokazuje nieźle co jest dziwne. Więc muszę to przemyśleć.

Siła wiatru zdecydowanie będzie lepiej oprogramowana bo nie znając możliwości wiatraczka błądzę w obliczeniach.

### Supla i jej możliwości

Niestety Supla nie ma jeszcze generycznego kanału, któremu można przypisać jednostkę lub kanału stacji pogodowej przewidującego więcej parametrów więc niektóre pomiary musiały zostać przepchnięte przez kanał temperatury, np. napięcie akumulatorków czy kierunek wiatru jako kąt z zakresu 0-360 względem północy.

## Plany na przyszłość

- Pewnie większy panel fotowoltaiczny.
- Pomiar napięcia z panelu.
- Pomiar poboru prądu przez ESP - mam już moduł INA219 obczajony :D
- Własne płytki zamiast breakout boardów, a przynajmniej ta zewnętrzna.
- Pomiar zanieczyszczenia powietrza (SDS0111).
- Po przekroczeniu ustawionej godziny przechodzić na tryb "deep_sleep" i budzić co 15 min na minutę aby pomiar wiatru i deszczu był choć trochę użyteczny. Następnie rano o ustalonej godzinie budzić się aby pracować do wieczora w celu wydłużenia pracy na akumulatorkach.
- Albo w Supli pojawi się kanał ogólnego przeznaczenia, który będzie wspieral dane, które mogę wysłać albo zgłosze PR ze zmianami :D
