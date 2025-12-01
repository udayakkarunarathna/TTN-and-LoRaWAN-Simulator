#!/bin/bash

APP_ID="my-lab2-ttn-appid-1"
DEVICE_ID="my-lab2-end-iot-dev1"
API_KEY="NNSXS.KY4AJ3OSDI3FBPWSJYYEFL2CRAHPSF643CYHWNY.SLMMMLSPWGNZXBEY6WMO533RL2SLC2K6SQRU5YOBJTYHK37VEJ6Q"

curl --location \
  --header "Authorization: Bearer $API_KEY" \
  --header 'Content-Type: application/json' \
  --header 'User-Agent: my-iot-integration/1.0' \
  --request POST \
  --data '{
    "downlinks": [{
      "frm_payload": "AQE=",
      "f_port": 15
    }]
  }' \
"https://eu1.cloud.thethings.network/api/v3/as/applications/$APP_ID/devices/$DEVICE_ID/down/push"
