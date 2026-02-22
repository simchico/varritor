# third_party dependency ledger

Vendored dependencies are copied into this directory and pinned.

For each dependency, record:
- Name
- Upstream URL
- Upstream commit/tag/version
- License
- Local path
- Notes on local modifications (if any)

## Template

| Name | Upstream URL | Commit/Tag | License | Local Path | Local Modifications |
|---|---|---|---|---|---|
| example-lib | https://github.com/example/example-lib | v1.2.3 | MIT | third_party/example-lib | none |

## Policy
- Do not fetch dependencies at build time.
- Keep upstream/license metadata current.
- If vendored code is modified, document exactly what changed and why.
