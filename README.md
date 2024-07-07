# mpi

This repo contains several mpi apps and algorithm based fault tolerant apps (mm, lu, kmeans).

- CRC32 is used to verify the integrity of communication data.
- Hamming code (SECDED) is used to correct one-bit errors and detect two-bit errors in data blocks. 

## MM
- Compile
```
make mm
```
- Run
```
mpirun -np <num_of_procs> bin/mm <matrix_a_filename> <matrix_b_filename>
```
- A small program [gen_matrices.py](src/mm/gen_matrices.py) helps to generate a matrix. 
- Requirement
    - m_1->cols == m_2->rows
    - m_1->rows % num_worker == 0

## LU
- Compile
```
make lu
```
- Run
```
mpirun -np <num_of_procs> bin/lu <matrix_size>
```

## K-means
- Compile
```
make kmeans
```
- Run
```
mpirun -np <num_of_procs> bin/kmeans [clusters] [max_iterations] [datafile] # (default) 100 1000 ./data/obs_info.txt
```

## Himeno
- Parameter Setting
```
cd src/himeno
./paramset.sh M 1 1 2 # generate new param.h
```
- Compile
```
make himeno
```
- Run
```
mpirun -np <num_of_procs> bin/himeno
```

