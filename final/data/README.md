# Datasets

This directory holds graph inputs. Two paths are supported by the code:

* tiny synthetic graphs committed here (e.g. `sample.edges`) for smoke tests
* large real-world graphs fetched via `download.sh` (not committed)

## Format

Plain text, one directed edge per line:

```
<src_id> <dst_id>
```

* IDs are non-negative integers.
* Lines beginning with `#` or `%` are ignored (SNAP-style comments).
* The number of vertices is inferred as `max(id) + 1` if not provided.

## Recommended SNAP datasets

| Name        | n        | m         | Notes                                |
|-------------|----------|-----------|--------------------------------------|
| web-Stanford| 281 903  | 2 312 497 | Small enough for laptop, real web    |
| web-Google  | 875 713  | 5 105 039 | Good OpenMP test size                |
| web-BerkStan| 685 230  | 7 600 595 | Good MPI test size                   |
| LiveJournal | 4.8M     | 69M       | Big enough for cluster runs          |

Use `./download.sh <name>` to fetch any of the above.
