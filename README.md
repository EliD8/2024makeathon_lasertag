# 2024makeathon_lasertag
## Inspiration

After playing laser tag as kids and recently rediscovering it as adults but not having any locations near us to play, our team looked for a solution that wasn't tied down to a physical location. We wanted to be able to play laser tag anywhere, anytime!

## What it does

Quick Connect Laser Tag allows for up to 20 players to pick up one of our custom-made laser blaster and sensor device, and allows players to engage in a fun game of laser tag in 2 different game modes!

Game Mode 1: Last One Standing In Last One Standing, all players have 10 lives. The last player with remaining lives is the winner

Game Mode 2: Time Deathmatch In Time Deathmatch. A game duration is set, and the player with the most eliminations at the end of the game is the winner.

## How we built it

We 3D printed a housing for the laser blaster which contains an ESP32, laser transmitter, and laser sensor. We designed the laser blasters to communicate with each other during gameplay over ESP32Now peer-to-peer communication to relay point scoring data. When the laser sensor detects a hit, it can send an acknowledgement to the blaster that made the hit in order to correctly assign points to players. We worked with an OLED display over I2C to show the user their remaining lives or points scored and game time remaining, depending on the game mode.

## Challenges we ran into

To implement game mode two, we initially tried to use the HDK Android development board to act as a server that could communicate with the individual laser guns to tally point totals, however we were unable to get the board to work with WiFi over the eduroam network, or enable Direct WiFi to the laser blasters.

## Accomplishments that we're proud of

We're proud that when we were unable to work with the HDK Android development board, we were able to pivot to a different technology that could enable us to get our blasters to communicate. We'd never used the ESPNow communication protocol, and successfully used it to connect a theoretical limit of 20 blaster over a field range of 200 yards!

## What we learned

ESPNow using MAC addresses
Asynchronously receiving packets
3D Printing
Working with OLED displays
Using lasers to trigger sensors over long distances


## What's next for Quick Connect Laser Tag

Building more blasters to support more players
Custom PCB
Improving blaster housing