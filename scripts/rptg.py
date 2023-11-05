# load random games from the playtak db
# cutted to a random length between 1/5 and 4/5 of the original moveset
# and convert the playtak moveset format to tps.
# needs the `pt2tps` c script built in `script/pt2tps/build`
# and the games.db playtak db

import subprocess
import csv
import sys
import io
from pathlib import Path
import random
import click


def load_sql(amount: int, s_min: int, s_max: int) -> str:
    return (
        subprocess.run(
            ["sqlite3", "data/games.db"],
            input=f"""
.headers on
.mode csv

SELECT games.id, games.size, games.notation
FROM games
WHERE
    LENGTH(games.notation) > 10                                 -- games with at least some moves
    AND (games.rating_black + games.rating_white) / 2 > 1500    -- games by decent players
    AND games.size >= {s_min} AND games.size <= {s_max}         -- games in the given size range
ORDER BY RANDOM()
LIMIT {amount}
            """.encode(),
            capture_output=True,
        )
        .stdout.decode()
        .strip()
    )


def cut_notation(notation: str):
    moves = notation.split(",")
    onefifth = int(len(moves) / 5)
    return ",".join(moves[: random.randint(onefifth, onefifth * 4)])


@click.command()
@click.argument("amount", type=int, default=100)
@click.argument("size_min", type=int, default=3)
@click.argument("size_max", type=int, default=8)
def main(amount: int, size_min: int, size_max: int):
    amount = amount if amount > 0 else 1
    size_min = size_min if 3 <= size_min <= 8 else 3
    size_max = size_max if 3 <= size_max <= 8 else 8
    size_min = size_min if size_min <= size_max else size_max

    # load sql
    raw = load_sql(amount, size_min, size_max)
    reader = csv.DictReader(raw.split("\n"))

    # write trimmed csv
    out = io.StringIO()
    writer = csv.DictWriter(out, reader.fieldnames)
    for row in reader:
        writer.writerow(dict(row, **{"notation": cut_notation(row["notation"])}))

    # compile c script
    subprocess.run(["make"], cwd=Path(__file__).parent / "pt2tps", capture_output=True)

    # convert notation to tps
    res = subprocess.run(
        ["./pt2tps"],
        cwd=Path(__file__).parent / "pt2tps/build",
        input=out.getvalue().encode(),
        capture_output=True,
    )
    print(res.stdout.decode().strip())


if __name__ == "__main__":
    main()
