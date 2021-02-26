# HashNodeTree

InariKVSではHashNode(HashNode-Treeという独自の木アルゴリズムを用いています。HNTそのものはソートは行わず、また追加された順序も無視します（ソートや追加順序が必要であるならばリンクリストなど別のアルゴリズムを組み合わせて使用してください）。

## 特徴

HNTはハッシュ値をどう生成するのかが全てといっても過言ではありません。ハッシュ値に偏りがあると全く性能を発揮できません。逆にハッシュ値がうまく分散されると非常に高速かつ簡単に実装できます。

#### ハッシュ値

HNTで使用するハッシュ関数は、元の文字列(S)と深さ(D)からハッシュ値(H)を求めることができるとより効率的にノードを分散して配置できます。

```c++
H = HASH_FUNCTION(S, D);
```

実際に使用する場合は深さＤは０から始まり１，２，３と順に増えていくので実際には深度Dは気にする必要がなく、ハッシュ値を更新しながら読み取れれば良いです。効率や安全性を無視し、概要だけ説明すると以下のようなプログラムです。

```c++
struct hash_function
{
    char const* mStr    = nullptr;
    char get() { return *mStr++; }
};
hash_function hf = { "HELLO WORLD" };
char depth1_hash = hf.get();
char depth2_hash = hf.get();
```

#### 基本構造

HNTでは枝と葉の区別は不要です。分けることも出来ますが、あまり意味がないので一緒に扱います。

また、各ノードは自身の値とノードサイズ(NODESIZE)と一致する子ノードのリストを持っています。

```c++
struct node
{
    std::string value;
    node*       children[NODESIZE];
	// 有効な子をひとつ返す。なければnullptr
    node** child()
    {
        for (int i = 0; i < NODESIZE; ++i)
            if (children[i])
                return &children[i];
        return nullptr;
    }
};
```





#### 検索

ノードを辿る場合は深度ごとにハッシュ値を求め、ハッシュ値に対応した子ノードに遷移します。プログラムを書くと次のようになります。多くの木構造に見られる分岐処理が無いぶん処理が非常にシンプルです。

```c++
hash_function  hf("Source string");
node*          current = root;
while (current)
{
    if (current->value == "Source string")
    {
        return current;  // 見つけた
    }
    current = current->children[hf.get() % NODESIZE];
}
```

#### 挿入

挿入はとても簡単です。ノードを辿り末端のノードの子に新しいノードを追加するだけです。

```c++
hash_function  hf("New string");
node**         current_ground = &root;
while (*current_ground)
{
    current_ground = &(*current_ground)->children[hf.get() % NODESIZE];
}
current_ground = new node("New string");
```

#### 削除

削除もとても簡単です。削除したいノードの子ノードの中にある末端のノードと入れ替えるだけです。

```c++
void del(node** _target)
{
    // 末端ノードを探し tail に格納する
    node** tail = _target;
    for (auto n = tail; n; n = (*n)->child())
    {
        tail = n;
    }
    (*tail)->children = (*_target)->children; // 子をコピー
    // 末端にあったノードを削除対象にあった位置に移動
    *_target = *tail;
    *tail = nullptr;
}

```

