# Calorie Tracker CLI (C + ncurses)

A terminal-based calorie tracker written in C using the **ncurses** library.
# Demo
If u want, you can download the video and watch
![Watch Demo Video](demo/demo.mp4)

This project helps users:

* calculate BMI
* estimate daily calorie intake
* track foods eaten during the day
* store food records with dates
* calculate total calories consumed
* delete food entries
* automatically save records to a file

---

# Clone the Repository

```bash
git clone https://github.com/lunorith/calorie-tracker-.git
cd calorie-tracker-
```

---

# Features

## `calorie.c`

* Calculates BMI
* Suggests daily calorie intake

## `food.c`

* Add foods eaten during the day
* Save meal type and date
* Automatically calculate calories
* Show all food entries
* Delete entries
* Display total calories consumed
* Save all records automatically in `tracker.txt`

---

# Technologies Used

* C
* ncurses
* shell scripting
* curl
* jq
* file handling

---

# Requirements

You need:

* gcc
* ncurses
* curl
* jq

---

# Installation

## Debian / Ubuntu Based

```bash
sudo apt update
sudo apt install gcc libncurses-dev curl jq
```

---

## Arch / Arch-Based (EndeavourOS, Manjaro, etc.)

```bash
sudo pacman -S gcc ncurses curl jq
```

---

## Fedora

```bash
sudo dnf install gcc ncurses-devel curl jq
```

---

## NixOS

```bash
nix-shell -p gcc ncurses curl jq
```

---

# Windows Users

Since this project uses `ncurses`, it is recommended to use:

* WSL (Windows Subsystem for Linux)
  OR
* MSYS2

You can still use the VS Code terminal with them.

## WSL Setup

Install WSL:
https://learn.microsoft.com/windows/wsl/install

Then run:

```bash
sudo apt update
sudo apt install gcc libncurses-dev curl jq
```

---

## MSYS2 Setup

Install:
https://www.msys2.org/

Then open the MSYS2 terminal and run:

```bash
pacman -S gcc ncurses curl jq
```

---

# Give Permission to Script

Before running the project, give executable permission to the shell script:

```bash
chmod +x calorie.sh
```

Run this command in the terminal inside the project folder.

---

# Compilation

Compile both files using:

```bash
gcc calorie.c -o calorie -lncurses
gcc food.c -o food -lncurses
```

---

# Run

```bash
./calorie
./food
```

---

# Files

| File          | Purpose                        |
| ------------- | ------------------------------ |
| `calorie.c`   | BMI and calorie recommendation |
| `food.c`      | Food tracking system           |
| `calorie.sh`  | Fetches calories using API     |
| `tracker.txt` | Stores food records            |

---

# Notes

* `ncurses` is used to create a colorful terminal interface.
* Some common foods use built-in calorie values for better accuracy.
* Unknown foods use API fetching through `calorie.sh`.
* All food records are automatically saved in `tracker.txt`.

---
