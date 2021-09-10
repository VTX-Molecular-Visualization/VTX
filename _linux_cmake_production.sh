#!/bin/sh
cmake -B build -DDEFINE_PRODUCTION=ON .
cmake --build build --config Release --clean-first
cp -R build/build RELEASE
rm -R RELEASE/libraries/representations
rm -R RELEASE/snapshots
rm -R RELEASE/logs
rm RELEASE/config.ini
rm RELEASE/setting.json
rm RELEASE/libraries/render_effects/Default