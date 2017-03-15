A simple database holds variable length records of a 'Device' class, using fit-first availList stratgey , data is saved on data.dat, a backup example available on backup.dat.

### Record structure:

Cell    | Bytes
-------- | ---
Size | 4 (int)
availList pointer    | 4 (int)
ID    | 0~20 [Max 20]
Delimiter('#')    | 1 (char)
Name    | 0~30 [Max 30]
Delimiter('#')    | 1 (char)
Brand    | 0~30 [Max 30]
Delimiter('#')    | 1 (char)
Price    | 4 [(float)


C++ / minGW
