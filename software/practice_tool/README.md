This is a practice tool to help locating notes on the board. It displays a note, and you must press the button for it. If right, it picks another note. If wrong, it says you made a mistake.

I've only tested on Mac. To install, first install MuseScore (which converts notes to PNG images). Then run:

```sh
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt

python3 -m music21.configure
```

Instructions for music21 on Windows/Linux are [here](https://music21.org/music21docs/installing/index.html).

Then run the UI:

```sh
python3 main.py
```

