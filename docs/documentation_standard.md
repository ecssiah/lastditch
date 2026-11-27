# Documentation standard

The same documentation standard applies to every C file in `src/` and `tests/`.

The standard answers four questions, always in the same place:

| Question | Where the answer is |
| --- | --- |
| What is this file for? | The file banner. |
| What is this data, and what are its keys, order and lifetime? | The block above the type or table. |
| What does each column, member or constant mean? | The comment directly above it. |
| What does this function do, and which data does it read and write? | The block above the function. |

## 1. Comment syntax

All comments are `//` comments. A file banner rule uses `// ` followed by
76 equals signs. A block rule uses `// ` followed by 76 hyphens.

```c
// ============================================================================
// ----------------------------------------------------------------------------
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
    // Number of rows stored.
    s32 row_count;
    ...
    // [PK] Key of the link.
    s32 *link_id;
    // [FK -> AreaTable.area_id] First area joined.
    s32 *area_1_id;
```

### Members, enumerators and constants

Every struct member, every enumerator and every `#define` in a `Constants`
group has a comment directly above its declaration, at the same indentation.
Column comments start with a tag when one applies:

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
void area_table_delete(AreaTable *table, s32 area_id);
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
the function declarator starts the next line. Write pointer stars with the
declarator, including for pointer returns. When a definition has multiple
parameters, put each on its own line and align them with the first parameter.

```c
// Documented in src/app/data/area.h.
void
area_table_delete(AreaTable *table,
                  const s32 area_id)
{
```

## 5. Comments inside functions

Comments inside a function body are free-form `//` comments. They explain
*why* the code does something (an invariant, an ordering requirement, a
non-obvious formula), not *what* the next line does.
