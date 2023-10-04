# load random games from the playtak db
# cutted to a random length between 1/5 and 4/5 of the original moveset
# and convert the playtak moveset format to tps.
# needs the `pt2tps` c script built in `script/pt2tps/build`
# and the games.db playtak db

import subprocess
import csv
import random
import io
import sys


def load_sql(limit: int) -> str:
    with open("scripts/rptg.sql") as sql:
        return (
            subprocess.run(
                ["sqlite3", "data/games.db"],
                input=sql.read().replace("__LIMIT__", str(limit)).encode(),
                capture_output=True,
            )
            .stdout.decode()
            .strip()
        )


def cut_notation(notation: str):
    moves = notation.split(",")
    onefifth = int(len(moves) / 5)
    return ",".join(moves[: random.randint(onefifth, onefifth * 4)])


if __name__ == "__main__":
    raw = load_sql(100 if len(sys.argv) == 1 else int(sys.argv[1]))
    reader = csv.DictReader(raw.split("\n"))

    # write trimmed csv
    out = io.StringIO()
    writer = csv.DictWriter(out, reader.fieldnames)
    for row in reader:
        writer.writerow(dict(row, **{"notation": cut_notation(row["notation"])}))

    # compile c script
    subprocess.run(["make"], cwd="scripts/pt2tps", capture_output=True)

    # convert notation to tps
    res = subprocess.run(
        ["scripts/pt2tps/build/pt2tps"],
        input=out.getvalue().encode(),
        capture_output=True,
    )
    print(res.stdout.decode().strip())
