#!/bin/bash

FOOD_NAME="$1"

ENCODED=$(printf '%s' "$FOOD_NAME" | jq -sRr @uri)

URL="https://world.openfoodfacts.org/cgi/search.pl?search_terms=${ENCODED}&search_simple=1&json=1"

RESPONSE=$(curl -s -A "Mozilla/5.0 (X11; Linux x86_64)" "$URL")

if [ -z "$RESPONSE" ]; then
    echo "-1"
    exit 1
fi

CALORIES=$(printf '%s' "$RESPONSE" | jq -r '
[
  .products[]?
  | select(.nutriments != null)
  | select(
      (.nutriments["energy-kcal_100g"] //
       .nutriments["energy-kcal"] //
       .nutriments["energy_100g"]) != null
    )
  | (
      .nutriments["energy-kcal_100g"] //
      .nutriments["energy-kcal"] //
      (.nutriments["energy_100g"] / 4.184)
    )
  | select(. > 0 and . < 900)
][0] // "-1"
' 2>/dev/null)

if [[ "$CALORIES" =~ ^[0-9]+(\.[0-9]+)?$ ]]; then
    printf "%.0f\n" "$CALORIES"
else
    echo "-1"
fi
