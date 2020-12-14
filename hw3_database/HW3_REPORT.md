# Key-Value Storages

<!-- ⅠⅡⅢⅣⅤⅥⅦⅧⅨⅩ -->

## 基本資料

| 學號      | 姓名   | 系級     |
| --------- | ------ | -------- |
| E14066282 | 溫梓傑 | 機械 110 |

---

:::info

## 開發環境

- **OS:** Ubuntu 18.04.4 LTS
- **CPU:** Intel® Core™ i5-6300HQ CPU @ 2.30GHz × 4
- **Memory:** 7.7 GiB
- **Swap:** 1.37GiB
- **Language:** C++ (standard 11)
- **Compiler:** g++ (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0
  :::

---

## 程式架構簡介

本程式是以 B+ Tree 為基礎，所建構的 Key-value storage 程式，使用 B+ Tree 的好處是，所有資料節點都在樹的最底層，適合連續存取，而且 B+ Tree 在插入、刪除的時間皆為 O(log n)，非常適合作為資料庫程式的基礎。

### 檔案結構

為了加速寫入與讀取速度，這次採用二進位格式來存取，一筆資料為`Key(8 bytes)+Value(128 bytes)`，共 136 bytes。

- 定義一個 Block 大約等於 4 KB，`4K/136 = 30`，因此一個 Block 有 30 筆資料。

- 定義一個檔案最多存有 500MB 的資料，500M/4K = 131,586，因此一個檔案最多 131,586 個 block

#### 檔案結構如下所示：

| Key Value Content            |
| ---------------------------- |
| -----(Cache start pos)------ |
| Max key for each block       |
| cache_beg_pos                |
| Min key for file             |
| Max key for file             |

### 檔案如何擴充

本人將檔案限制其大小約 500M 左右，一個檔案一旦偵測到過大，它會自己分裂成兩個半滿的檔案。

### Cache 機制

當資料庫在初始化時，會讀取每個檔案的 Max key for each block ，來提昇 GET 指令的速度。

### PUT 指令

![](https://i.imgur.com/DIjvOBH.png)

### GET 指令

![](https://i.imgur.com/6j7JM98.png)

### B+ Tree insertion

程式碼參考自
https://www.geeksforgeeks.org/insertion-in-a-b-tree/
P.S. 其實那個網站的程式是有**很多**BUG 的
![](https://i.imgur.com/XtpF3NZ.png)

### LRU algorithm implementation

當記憶體不足時(假設 4GB)，LRU 演算法會把最近最少用(PUT)的檔案，dump 至 disk 以節省記憶體資源。
但是在 dump 的過程中，會大量消耗 I/O 資源，且程式執行會被卡住。

程式碼參考自
https://www.geeksforgeeks.org/lru-cache-implementation/

<!-- ![](https://i.imgur.com/DWvBdYL.png) -->

## 程式執行時間

| PUT instr. num | Execution time | \*.file number |
| :------------- | -------------- | -------------- |
| 10             | 258 ms         | 1              |
| 100            | 1,188 ms       | 1              |
| 1,000          | 18,469 ms      | 1              |
| 10,000         | 68,453 ms      | 1              |
| 100,000        | 512,485 ms     | 1              |
| 1,000,000      | 5,023,911 ms   | 2              |
| 10,000,000     | 57,848,985 ms  | 5              |

![](https://i.imgur.com/U0bj75l.png)

::: success
**[圖一]CPU time v.s. PUT instruction number**
可以發現在 並且在[圖一]中，大致上與 nlogn 理論值相符。
:::

## 程式開發與使用說明

### How to run code

```console=1
# Compile
$ make
# Run
$ ./hw3.out [./...(path)/*.input_file]
```

---

## 效能分析報告

#### 程式執行期間各個 Stage 對電腦資源使用情形

![](https://i.imgur.com/mqaOILA.png)

:::warning
**[圖三]、[圖四] 不同階段使用硬體資源之狀況**

| Stage           | CPU        | Memory     | I/O        |
| :-------------- | ---------- | ---------- | ---------- |
| start. 程式開始 | high usage | high usage | low usage  |
| A. Dump to disk | high usage | high usage | high usage |
| B. 程式結束     | low usage  | low usage  | low usage  |

:::

#### 結論

從以上實驗可以看出，作業系統在背後做了非常多優化服務，包括 CPU 資源分配(即行程管理)、記憶體管理、虛擬記憶體管理、Disk I/O 管理等。
本次作業最花時間的地方在於一開始的規劃，如何從外部檔案快速找到目標檔案，找到檔案後，還要快速尋找檔案中的資料，後來決定用 Cache 的方法實做，從摸索到完成整個概念的設計就花了不少時間。
由於程式的瓶頸往往是 I/O 處理速度，因此如何減少 I/O 的次數是改善程式速度的最大關鍵。

### 改善之處

本程式還有很多改善的空間，在此提出一些可以改善之處：

1. 在不會發生 Data Race 的情況下，加入平行處理(雖然我認為這個改善效果不會很顯著)
2. 先『偷看一些』未來的指令，把指令 Rearrange，再統一對資料庫進行操作。
3. 本程式在 Cache 的部份其實還有些 BUG，因為在資料庫初始化時，會強制讀取所有檔案的 Cache，因此若檔案過多，Cache 可能會導致記憶體空間不足。

## Contact

### :email: e14066282@gs.ncku.edu.tw
