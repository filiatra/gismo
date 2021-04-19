# Run gismo files
echo "P = 3, R = 1"
#echo ./bin/biharmonic_argyris_example -g -1 -l 4
#./bin/biharmonic_argyris_example -g -1 -l 4


for pp in 3 4
do
    for i in 1100 1110 1120 1122
    do
    echo ./bin/biharmonic_argyris_example -g $i -p $pp -r 1 -l 6 --csv
    ./bin/biharmonic_argyris_example -g $i -p $pp -r 1 -l 6 --csv

    echo ./bin/biharmonic_argyris_example -g $i -p $pp -r 1 -l 6 --interpolation --csv
    ./bin/biharmonic_argyris_example -g $i -p $pp -r 1 -l 6 --interpolation --csv
    done
done


#for i in 1110 1120 1122
#do
#echo ./bin/biharmonic_argyris_example -g $i -p $pp -r 1 -l 0 --solution --mesh
#./bin/biharmonic_argyris_example -g $i -p $pp -r 1 -l 0 --solution --mesh
#done

echo Finished!
