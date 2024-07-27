# About the datasets

In our paper, we use four datasets to conduct the experiments (CAIDA, Synthetic, amazon and dblp).  
- **CAIDA Anonymized Internet Trace** is a data stream of anonymized IP trace collected in 2018. Each packet is identified by its source IP address (4 bytes), source port number (2 bytes), destination IP address (4 bytes), destination port number (2 bytes), and protocol type (1 bytes). Each packet is associated with a 8-byte timestamps of double type. 

- **Synthetic Dataset** is a synthetic dataset generated with flow cardinalities following a Zipf-1.0 distribution.

- **Amazon Dataset** is collected by crawling the Amazon
website. It is based on the ¡±Customers Who Bought This Item
Also Bought¡± feature of the Amazon website. If a product i is
frequently co-purchased with product j, the graph contains an
undirected edge from i to j. Each product category provided
by Amazon defines each ground-truth community.

- **DBLP Dataset** is derived from the DBLP computer science
bibliography, which provides a comprehensive list of research
papers in computer science. We construct a co-authorship
network where two authors are connected if they publish at
least one paper together. The publication venue, e.g., a journal
or conference, defines an individual ground-truth community;
authors who publish in a certain journal or conference form a
community.

Here, we provide two small sample datasets:

- A dataset shard extracted from the real-world CAIDA dataset (`./demo.dat`). For the full CAIDA datasets, please register in [CAIDA](http://www.caida.org/home/) first, and then apply for the traces. 

- A dataset shard extracted from our synthetic dataset `syn.dat`. You can download the full dataset file [here](https://drive.google.com/file/d/1jau6Yc4H4wrYvj-c9Ci1XXnI_1ICdlsm/view?usp=sharing). This is a dataset genreated with the opensource tool [webpolygraph](http://www.web-polygraph.org/). 

- The Amazon and DBLP datasets are available for download at the Stanford Network Analysis Project (SNAP) website: [Stanford SNAP Datasets](https://snap.stanford.edu/data/index.html).
