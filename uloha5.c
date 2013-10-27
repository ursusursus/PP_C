Ukladanie do hash tabulky
1. input je pole stringov na roote
2. hasTable bude rozmeru maxAsciiValue (126) * MAX_STRING_LENGTH
2. rozscatterujem input na procesy
3. paralelne vypocitat hashe pre stringy, hash = suma ascii hodnot pre znaky;
4. gathernut hashe
5. setnut hashe ( = indexy) do ht na roote
