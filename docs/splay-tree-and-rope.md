# スプレー木とロープの使い方

### スプレー木

TBD

### ロープ

TBD

現状では `t_rope` は単なる `t_splay` の別名なので、`splay_` で始まる関数はすべて `t_rope` にも適用できる。

### メモリ管理の鉄の掟

メモリリークを防ぐために参照カウンタが使われている。スプレー木のデータを扱うときには参照カウンタが正しく設定されるように、以下の API を使う。

#### splay_init()

`t_splay*` 型の変数を NULL 以外の値で初期化するときには `splay_init()` を使う。

この関数は、代入する木の参照カウンタをインクリメントする。

```
        t_rope  *rope;

        splay_init(&rope,
                   rope_concat(rope_create("a", NULL),
                               rope_create("b", NULL)));
```

#### splay_assign()

すでに何かの値が格納された `t_splay*` 型の変数に再度別の値を代入するときは `splay_assign()` を使う。

この関数は変数がもともと指していた木の参照カウンタをデクリメントし、新しく代入される木の参照カウンタをインクリメントする。

```
        splay_assign(&rope, rope_insert(rope, 12, lazy));
```

#### splay_release()

ローカル変数として宣言した `t_splay*` 型の変数は、スコープから抜ける前に `splay_release()` を適用する。

この関数は木の参照カウンタをデクリメントし、その結果カウンタが 0 になったらメモリを解放する。

#### 関数から木を返す場合

関数の返り値として `t_splay*` 型の値を返すときは、参照カウントのつじつまを合わせるために `return` の直前で `refcount` をデクリメントする。

```
    t_rope  *rope;
...
            rope->refcount--;
            return (rope);
```

ここで `splay_release()` を使わないように注意する。`splay_release()` を呼ぶと、参照カウンタが 0 の場合にメモリが解放されてしまうので、呼び出し元に正しく値が返らないからである。

呼び出し元では上記の `splay_init()` または `splay_assign()` が使われるので、必ず参照カウンタが 1 以上になるから、ここではメモリを解放してほしくない。