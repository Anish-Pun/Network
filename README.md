# UnoReverse

## Inhoud van opdracht
Schrijf een programma met naam "UnoReverse" dat bestaat uit een TCP-server en HTTP-client. De TCP server luistert op poort 22 (default ssh) naar nieuwe verbindingen en achterhaalt het IP-adres van de aanvaller (i.e. bot-net). Hij logt de inlogpoging in een logbestand samen met de ontvangen gegevens, netwerkstatistieken en de geo-locatie m.b.v. IP Geolocation API. Voor dit laatste maakt het programma gebruik van een HTTP-client om de API aan te spreken en de gegevens die terugkomen in de log te bewaren. Verder zal de server een omgekeerde aanval uitvoeren door een grootte hoeveelheid aan data terug te sturen.

## Functies
- ✔️ TCP server luistert op poort 22.
- ❌ TCP server accepteert meerdere verbindingen.
- ✔️ TCP server achterhaalt IP adres van client.
- ✔️ TCP server logt IP adres in logs.
- ✔️ TCP server start een HTTP client per nieuwe verbinding.
- ✔️ HTTP client maakt een TCP verbinding met IP Geolocation API.
- ✔️ HTTP client stuurt een correcte HTTP GET request.
- ✔️ HTTP client ontvangt de reactie van de HTTP server.
- ✔️ HTTP client extraheert zuiver de volledige json reactie.
- ✔️ HTTP client logt de geolocatie in logs.
- ✔️ HTTP client sluit de verbinding zuiver af.
- ✔️ TCP server accepteert gegevens van client en logt ze in logs.
- ✔️ TCP server stuurt zoveel mogelijk gegevens naar de openstaande verbinding.
- ✔️ TCP server houdt een teller bij hoeveel gegevens succesvol zijn afgeleverd en logt deze bij het sluiten van de verbinding in logs.
- ✔️ TCP server sluit de verbinding nadat de client de verbinding sluit.
- ❌ TCP server kan meerdere verbindingen simultaan verwerken.
- ✔️ Het geheel werk volledig zonder crashen en er wordt efficiënt met de resources (i.e. memory en sockets) van de computer gebruik gemaakt.
- ✔️ Code staat professioneel op GitHub (i.e. meerdere nuttige commits en uitleg).
- ✔️ Het geheel werkt en is beschikbaar op het internet (e.g. thuis m.b.v. NAT of werkt op publieke server).
##Een mooie extra eens de rest werkt (zie de lijst hieronder) :
  - ❌ Correcte documentatie a.d.h.v. flowchart (zie miro)
  - ✔️ Maak gebruik van pthread
  - ❌ IP's worden in een look-up tabel bewaard (e.g. linked-list) om herhalende aanvallen te kunnen loggen.

## How to use.
    1. Compile it by using " gcc (name.c) -l ws2_32 -o (name.exe)
    2. ./(name.exe)
### To test the program.
    - Run packetsender and put on address " ::1 " and port " 22 ".