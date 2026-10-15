# OSSim

OSSim is a C++ operating-system scheduling simulator that saves experiment,
process, event, and scheduling-result records to Oracle Database.

## Project layout

- `SmartOS_Engine/main.cpp` — sample FCFS and Round Robin runs.
- `SmartOS_Engine/config/db_config.hpp` — Oracle credentials and connect string.
- `SmartOS_Engine/models/os_types.hpp` — process, event, and result models.
- `SmartOS_Engine/database/` — Oracle OCCI database manager.
- `SmartOS_Engine/scheduling/` — scheduler base class, FCFS, and Round Robin.

## Build and run

Install Oracle Instant Client Basic and SDK packages, then set the credentials
and connect string in `SmartOS_Engine/config/db_config.hpp`. With the Instant Client paths
available, build from the repository root with:

```sh
g++ -std=c++17 -I"$ORACLE_HOME/sdk/include" \
  SmartOS_Engine/main.cpp SmartOS_Engine/database/db_manager.cpp \
  -L"$ORACLE_HOME" -locci -lclntsh -o ossim
```

Run `./ossim` after starting an Oracle database reachable at the configured
connect string. The application creates its tables if they do not already
exist.
