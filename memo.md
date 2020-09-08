
## References
https://w.atwiki.jp/babais/pages/19.html
https://steamcommunity.com/sharedfiles/filedetails/?id=1861849340
https://w.atwiki.jp/babais/pages/42.html
https://babaiswiki.fandom.com/wiki/Category:Properties


## 現在のコードの特徴・問題点
- 暗黙のルール，Text is PUSHの処理の仕方が散らかっている
- 複数種類のMoveがProcessMoveに統合されており，移動の優先順位が不明瞭
   * Memo: 移動には優先順位があるが，その間でルールの再評価は行われない.再評価が行われるのは全ての移動が終わった後． (e.g., stage 2-10, MoveしているCogがStopであるRobotの1マス手前まで来た状態でYouがRobot is Stopを崩しても，CogはRobotに重ならず反転する．)
- ルールによる物体の変化が未実装
- Youが複数になることを想定していない
- Textとなにかが重なることを想定していない 
  * Textが存在するマスのObjectの0番目がTextであることを仮定したコードが散見される
  * Textが動いた後それの前提が崩れるけど？
- Shut, Openに対応する特定のタイプのエンティティ一つ削除がない

- それぞれのエンティティごとにインスタンスがあるのではなく，それぞれのマスごとにその上に存在するエンティティ全てを一つのObjectクラスとして管理している．
  * 同じマスの上にある各エンティティの状態を管理しにくい．
     * それはやりよう次第？現在は，ある操作に該当する関数をそのマスのエンティティ全体に適用させて，その中でルールに当てはまるものに対して実際の効果を適用するという形で行っている．
  * 向きに該当するメンバが無い．追加予定．

- 各エンティティはGameEnumで定義されるObjectTypeをIDとしており，そのIDは画像と一対一対応．例えば, 画面上に存在するBaBaは ObjectType::BABAではなく ObjectType::ICON_BABAである（前者はTextのBaba）．そのため，ルール適用の際にConvertIconToTextでアイコンを変換している．

- ルールは3つのObjectインスタンスの組．厳密にはteleを使うことによってテキスト同士が重なることもある (stage ABC-4）ので，現在のパース形式では1つのObjectにつきTextが最大1つであることを想定しているため完全とは言えないが，レアケースだし将来的にはパースの仕方を変えるだけで現状問題無さそう？

- ルールを適用する際の処理が若干変則的．まず， Map.At(x, y).GetTypes() でそのマスのObjectTypeを全て列挙し，RuleManager.HasProperty(types, ObjectType::XXX) を適用することで，特定のプロパティに当てはまるObjectTypeをそのマスのObjectのどれかが持っているか？という形で判定を行う. 