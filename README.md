DictParser
==========

A dict parser which allows users define their own type and parser function
实现了一个通用的词表解析读取工具类(一个或者几个class/struct组成)，
解析并读取如下形式的词表：

   <col1>\t<col2>\t...\t<coln>

每一行都是一条记录，每一列可能的类型包括：

   * int
   * float
   * char *
   * 形如num:item1,item2,item3的数组（可以理解为T[num]的数组，T是用户给出某种build-in类型）
   * 其他用户自定义类型(需要灵活支持多种自定义类型)

每一列的数据中均不包含\t字符，每一行以\n结尾。

编译说明：
    * 编译库libdict_parser.a: make clean; make;
    * 编译运行demo: cd demo; make clean; make; ./demo
    * 编译运行unitest: cd unitest; make clean; make; cd output; ./test_parser 2>log
