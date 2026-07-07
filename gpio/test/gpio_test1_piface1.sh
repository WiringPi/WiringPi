#!/bin/bash

echo -e "🔧 Starting relay unit test for MCP23S17@Piface unit test hw\n\n"

# set pin 200–207 as output
for i in {200..207}; do
  gpio -x mcp23s17:200:0:0 mode $i out
done

# set pin 208–215 as input
for i in {208..215}; do
  gpio -x mcp23s17:200:0:0 mode $i in
  gpio -x mcp23s17:200:0:0 mode $i up
done


# Define relay output addresses and corresponding feedback input addresses
relay_addresses=(200 201 207 206)
feedback_addresses=(214 215 212 213)

for round in {1..3}; do
echo -e "🔄 Round $round / 3\n"

# Loop through each relay
for i in "${!relay_addresses[@]}"; do
    out=${relay_addresses[$i]}
    in=${feedback_addresses[$i]}

    echo "➡️  Relay/IO write test $((i+1)): Address $out → Feedback at $in"

    # Turn ON
    gpio -x mcp23s17:200:0:0 write "$out" 1
    sleep 0.5
    feedback_on=$(gpio -x mcp23s17:200:0:0 read "$in")

    if [ "$feedback_on" -eq 0 ]; then
        echo "✅ ON Test passed: Feedback is LOW as expected."
    else
        echo "❌ ON Test failed: Feedback is HIGH."
    fi

    # Turn OFF
    gpio -x mcp23s17:200:0:0 write "$out" 0
    sleep 0.5
    feedback_off=$(gpio -x mcp23s17:200:0:0 read "$in")

    if [ "$feedback_off" -eq 1 ]; then
        echo "✅ OFF Test passed: Feedback is HIGH as expected."
    else
        echo "❌ OFF Test failed: Feedback is LOW."
    fi

    echo -e "\n"
    sleep 0.5

done
    for addr in {208..211}; do

    echo "🔃 Pullup resistor test address $addr: "
     gpio -x mcp23s17:200:0:0 mode "$addr" up
    sleep 0.5
     feedback_on=$(gpio -x mcp23s17:200:0:0 read "$addr")

    if [ "$feedback_on" -eq 1 ]; then
        echo "✅ PULLUP Test passed: Feedback is HIGH as expected."
    else
        echo "❌ PULLUP Test failed: Feedback is LOW."
    fi
    sleep 0.5
 
     gpio -x mcp23s17:200:0:0 mode "$addr" down 
    sleep 0.5
     feedback_on=$(gpio -x mcp23s17:200:0:0 read "$addr")

    if [ "$feedback_on" -eq 1 ]; then
        echo "✅ PULLDOWN Test passed: Feedback is LOW as expected."
    else
        echo "❌ PULLDOW Test failed: Feedback is HIGH."
    fi

    echo -e "\n"
    sleep 0.5


done

done

#echo "🧹 Cleanup complete. All relays turned OFF."

