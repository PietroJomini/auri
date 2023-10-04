.headers on
.mode csv

SELECT games.id, games.size, games.notation
FROM games
WHERE
    LENGTH(games.notation) > 10                                 -- games with at least some moves
    AND (games.rating_black + games.rating_white) / 2 > 1500    -- games by decent players
ORDER BY RANDOM()
LIMIT __LIMIT__