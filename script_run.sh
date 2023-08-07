#!/bin/sh

m_directory_W="C:/CS255Project/3DConvolution/KaggleDataset/Matrices/"
m_directory="/cygdrive/c/CS255Project/3DConvolution/KaggleDataset/Matrices/"
m_files=$(ls "$m_directory")

k_directory_W="C:/CS255Project/3DConvolution/KaggleDataset/Kernels/"
k_directory="/cygdrive/c/CS255Project/3DConvolution/KaggleDataset/Kernels/"
k_files=$(ls "$k_directory")

<<comment
for k_file in $k_files
do
	k_length="${#k_file}"
	k_index=$((k_length - 7))
	echo $k_index
	k_height="${k_file:k_index:1}"
	k_index=$((k_index + 2))
	k_width="${k_file:k_index:1}"
	echo "$k_height"
	echo "$k_width"

	echo "$k_file" | tee -a log.txt
	for m_file in $m_files
	do
		echo "$m_file"
		mat_files=$(ls "$m_directory$m_file")
		fileSeparator="/"
		m_file="$m_file$fileSeparator"
		for file in $mat_files
		do
			echo "$file"
			length="${#file}"
			index=$((length - 11))
			height="${file:index:3}"
			index=$((index + 4))
			width="${file:index:3}"
			echo "$height"
			echo "$width"
			out_file="output_"
			file_ext=".txt"
			echo "$m_directory_W$m_file$file" | tee -a log.txt
			./conv.o "$m_directory_W$m_file$file" $height $width 3 "$k_directory_W$k_file" $k_height $k_width 3 "$out_file$height$file_ext" | tee -a log.txt
			./p_conv.o "$m_directory_W$m_file$file" $height $width 3 "$k_directory_W$k_file" $k_height $k_width 3 "$out_file$height$file_ext" | tee -a log.txt
			./m_conv.o "$m_directory_W$m_file$file" $height $width 3 "$k_directory_W$k_file" $k_height $k_width 3 "$out_file$height$file_ext" 8 | tee -a log.txt
		done
	done
done
comment

kernel_file="C:/CS255Project/3DConvolution/Sobel_11_11.txt"
echo "$kernel_file" | tee -a log.txt
for m_file in $m_files
do
	echo "$m_file"
	mat_files=$(ls "$m_directory$m_file")
	fileSeparator="/"
	m_file="$m_file$fileSeparator"
	for file in $mat_files
	do
		echo "$file"
		length="${#file}"
		index=$((length - 11))
		height="${file:index:3}"
		index=$((index + 4))
		width="${file:index:3}"
		echo "$height"
		echo "$width"
		out_file="output_"
		file_ext=".txt"
		echo "$m_directory_W$m_file$file" | tee -a log.txt
		./conv.o "$m_directory_W$m_file$file" $height $width 3 "$kernel_file" 11 11 3 "$out_file$height$file_ext" | tee -a log.txt
		./p_conv.o "$m_directory_W$m_file$file" $height $width 3 "$kernel_file" 11 11 3 "$out_file$height$file_ext" | tee -a log.txt
		./m_conv.o "$m_directory_W$m_file$file" $height $width 3 "$kernel_file" 11 11 3 "$out_file$height$file_ext" 8 | tee -a log.txt
	done
done

