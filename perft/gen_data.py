# run the given positions against the perft scripts
# requires https://github.com/tqdm/tqdm

"""
- do stats
    - nodes counts
    - deltas on nodes
    - benchmarks
        - nodes per second (mean)
"""

import asyncio
from pathlib import Path
from os import environ
from multiprocessing import Pool, RLock
from typing import List, Tuple

from tqdm import tqdm
from tqdm.asyncio import tqdm as atqdm

data = "data"
positions = Path(__file__).parent / data / "5.tps"
n_positions = positions.read_text().count("\n")
perft_upto = 4


class Engine:
    @classmethod
    def rust(cls, name: str, home: str):
        return cls(name, home, "cargo build --release", f"target/release/{home}")

    def __init__(self, name: str, home: str, build: str, target: str):
        self.name = name
        self.home = Path(__file__).parent / home
        self.target = self.home / target
        self.out = Path(__file__).parent / data / f"{self.name}.csv"
        self.buildc = build

    async def build(self):
        process = await asyncio.create_subprocess_shell(
            f"cd {self.home} && {self.buildc}",
            stdout=asyncio.subprocess.PIPE,
            stderr=asyncio.subprocess.PIPE,
        )

        await process.communicate()
        if process.returncode:
            raise Exception(f"{self.name} can't build: {process.stderr}")

    async def read(self, stream: asyncio.streams.StreamReader, i: int):
        # open out file
        with self.out.open("a") as out:
            with tqdm(
                total=n_positions,
                desc=f"Run [ {self.name} ]",
                position=i,
                dynamic_ncols=True,
            ) as bar:
                while line := await stream.readline():
                    out.write(line.decode())
                    bar.update()

    async def run(self, i: int):
        # ensure out file exists
        self.out.touch(exist_ok=True)
        self.out.write_text("")

        # run the engine
        process = await asyncio.create_subprocess_shell(
            str(self.target),
            stdin=positions.open(),
            stdout=asyncio.subprocess.PIPE,
            stderr=asyncio.subprocess.STDOUT,
        )

        # collect the data
        await self.read(process.stdout, i)
        await process.communicate()

    async def clean(self):
        self.out.unlink()


def run(p: Tuple[int, Engine]):
    return asyncio.run(p[1].run(p[0]))


async def prelude(engines: List[Engine]):
    environ["PERFT_UPTO"] = str(perft_upto)

    # build
    await atqdm.gather(*(e.build() for e in engines), desc="Build")

    # run the engines
    tqdm.set_lock(RLock())
    pool = Pool(initializer=tqdm.set_lock, initargs=(tqdm.get_lock(),))
    pool.map(run, enumerate(engines))


async def clean(engines: List[Engine]):
    await atqdm.gather(*(e.clean() for e in engines), desc="Clean")


if __name__ == "__main__":
    engines = [
        Engine("take4", "take4_perft", "make", "build/perft"),
        Engine("take4u", "take4u_perft", "make", "build/perft"),
        # Engine.rust("tiltak", "tiltak_perft"),
        # Engine.rust("topaz", "topaz_perft"),
    ]

    asyncio.run(prelude(engines))
    # asyncio.run(clean(engines))
