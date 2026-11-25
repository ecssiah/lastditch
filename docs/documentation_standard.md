# Documentation standard

Every C file in `src/` and `tests/`, and every generated C file, is documented
the same way. The rules below are the whole standard. They are enforced by
`tools/python/check_documentation.py`, which runs as the `documentation` test
of `ctest`; a file that breaks a rule fails the build's checks.

The standard answers four questions, always in the same place:

| Question | Where the answer is |
| --- | --- |
| What is this file for? | The file banner. |
| What is this data, and what are its keys, order and lifetime? | The block above the type or table. |
| What does each column, member or constant mean? | The trailing comment on its line. |
| What does this function do, and which data does it read and write? | The block above the function. |

## 1. Comment syntax

All comments are `//` comments. Two rule lines delimit documentation:

```c
// ============================================================================   file banner rule  ("// " + 76 "=")
// ----------------------------------------------------------------------------   block rule        ("// " + 76 "-")
```

A documentation block is: a block rule, a header line `// <Kind>: <Name>`,
one or more field lines `// <Field>: <text>`, and a closing block rule. The
text of the header and of every field starts in the same column. Text that
does not fit on one line continues on following lines, aligned with the text
above it. The thing being documented starts on the line
directly below the closing rule, with no blank line in between.

## 2. File banner

Every file starts with a banner with exactly these fields, in this order:

```c
// ============================================================================
// File:    src/app/world.h
// Layer:   app
// Purpose: The world database (cells, areas, links and their indexes), ...
// ============================================================================
```

* `File` is the path of the file relative to the repository root (generated
  files use `generated/<name>`).
* `Layer` is one of `core` (no knowledge of the game), `app` (the game),
  `platform` (the operating system boundary) or `test`.
* `Purpose` says what the file is responsible for.

## 3. Data structures

Every type, table, constant group and macro has a block. The header line
names a **kind**; the kind decides which fields the block must have.

| Kind | Used for | Fields, in order |
| --- | --- | --- |
| `Table` | A runtime table: a struct of column arrays and a row count. | `Purpose`, `Key`, `Order`, `Lifetime` |
| `Index` | A lookup structure derived from one or more tables. | `Purpose`, `Key`, `Order`, `Lifetime` |
| `Reference` | A read-only table compiled into the program (`const` array, or a group of `const` values). | `Purpose`, `Key`, `Order`, `Lifetime` |
| `Record` | One row as a value: the row type of a reference table, or a tuple passed to an insert function or to the GPU. | `Purpose` |
| `Value` | A small value type that is passed and returned by value (vectors, colors). | `Purpose` |
| `State` | Singleton state: a struct of which one instance exists, or that instance itself. | `Purpose` |
| `Enum` | An enumeration: the domain of a column or the key of a reference table. | `Purpose` |
| `Type` | Any other `typedef` (scalar aliases, function pointer types). | `Purpose` |
| `Constants` | A group of related `#define` constants. | `Purpose` |
| `Macro` | Function-like macros. | `Purpose` |

Every kind may end with an optional `Notes` field.

The four fields of the tabular kinds mean:

* `Purpose` – what one row represents and why the table exists.
* `Key` – the primary key: which column or columns identify a row, and
  whether the key is a surrogate issued by a `Sequence`, a natural key, or
  implicit in the row number.
* `Order` – the order in which rows are stored, and what depends on it.
* `Lifetime` – which function fills the table, when it changes, and which
  function releases it. For an index: what it is derived from and when it
  becomes stale.

```c
// ----------------------------------------------------------------------------
// Table:    LinkTable
// Purpose:  Connections between pairs of areas that share a border ...
// Key:      link_id, a surrogate key issued by id_sequence
// Order:    clustered on link_id, ascending
// Lifetime: filled by world_calculate_links; read-only afterwards; released
//           by link_table_free
// ----------------------------------------------------------------------------
typedef struct LinkTable
{
    s32 row_count;                      // Number of rows stored.
    ...
    s32* link_id;                       // [PK] Key of the link.
    s32* area_1_id;                     // [FK -> AreaTable.area_id] First area joined.
```

### Members, enumerators and constants

Every struct member, every enumerator and every `#define` in a `Constants`
group has a trailing comment on its own line. Column comments start with a
tag when one applies:

| Tag | Meaning |
| --- | --- |
| `[PK]` | The column is (part of) the primary key. |
| `[FK -> Table.column]` | The column holds keys of another table. |
| `[derived]` | The column is computed from other data and can be recomputed. |

A block covers the declaration directly below it. `Constants`, `Macro`,
`Type` and `Reference` blocks additionally cover the declarations of the same
sort that follow without a blank line (for example the eight `extern const
Color` lines under `Reference: named colors`); a `Constants` or `Macro` block
covers every `#define` up to the next block or declaration.

## 4. Functions

Every function is documented **exactly once**, with exactly these fields in
this order:

```c
// ----------------------------------------------------------------------------
// Function: area_table_delete
// Purpose:  Deletes the area with a given key, keeping the row order.
// Params:   table   - table to delete from
//           area_id - key of the row to delete; must exist
// Returns:  nothing
// Reads:    table
// Writes:   table: every column, row_count, revision
// Notes:    Links are not cascaded; the caller must not leave links that
//           refer to the deleted area.
// ----------------------------------------------------------------------------
void area_table_delete(AreaTable* table, s32 area_id);
```

* `Purpose` – what the function does, in terms of the data it transforms.
* `Params` – one item per parameter, in declaration order, written
  `name - meaning`; the names must match the declaration. `none` when the
  function takes no parameters.
* `Returns` – the meaning of the return value, or `nothing`.
* `Reads` – the tables, columns, reference tables, files or other state the
  function reads. `arguments only` for a function of its arguments alone;
  `nothing` when it reads nothing at all.
* `Writes` – the tables, columns or other state the function changes, or
  `nothing`.
* `Notes` – optional: preconditions, complexity, ordering guarantees.

`Reads` and `Writes` are the heart of the block: together they state which
part of the database a system touches, which is what a reader needs to know
to reorder, batch or parallelise systems safely.

### Where the block goes

* A function declared in a header is documented **at its prototype in the
  header**. Its definition carries no block; instead the line directly above
  the definition's return type reads `// Documented in <path of header>.`
* A function that is private to a `.c` file (`static`, and `main`) is
  documented **at its definition**.

Function definitions are written with the return type on its own line so that
the function name starts the line:

```c
// Documented in src/app/data/area.h.
void
area_table_delete(AreaTable* table, const s32 area_id)
{
```

## 5. Comments inside functions

Comments inside a function body are free-form `//` comments. They explain
*why* the code does something (an invariant, an ordering requirement, a
non-obvious formula), not *what* the next line does.

## 6. What the checker verifies

`check_documentation.py` understands the layout described above and reports:

* a missing or malformed banner, a banner whose `File` is not the file's path,
  or an unknown layer;
* a block with an unknown kind, with fields missing, out of order, repeated
  or misaligned, or not directly followed by a declaration;
* a `typedef struct`/`typedef enum` without a block of a matching kind, or
  whose block names a different type;
* any other `typedef` without a `Type` block;
* a `#define` (other than an include guard) that is not covered by a
  `Constants` or `Macro` block, or a constant without a trailing comment;
* a struct member or enumerator without a trailing comment;
* a reference table or file-scope variable without a `Reference` or `State`
  block;
* a header prototype or `static` function without a `Function` block, a block
  that names a different function, or a `Params` field that does not list
  exactly the function's parameters in order;
* a non-`static` function that no header declares, a definition that lacks
  its `// Documented in ...` line, or a function documented twice.

Run it by hand with:

```sh
python3 tools/python/check_documentation.py --root . src tests build/generated
```

It prints the number of files and blocks checked, broken down by kind.
