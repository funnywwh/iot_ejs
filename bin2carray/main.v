import os
import strings
fn usage(){
	
}
fn main(){
	if os.args.len < 3 {
		usage()
		return 
	}
	source_filename := os.args[1]
	header_filename := source_filename.replace('.c','.h')
	files_paths := os.args[2..]
	mut macro_name :=  header_filename.replace('.','_')
	macro_name = '__${macro_name}__'
	mut c_src := strings.Builder{}
	mut c_header := strings.Builder{}

	c_header.write('#ifndef $macro_name\n')
	c_header.write('#define $macro_name\n')
	c_src.write('#include "${header_filename}"\n')
	for path in files_paths{
		fc := os.read_bytes(path) or {
			continue
		}
		mut arrayname := path.replace(".","_")
		arrayname = arrayname.replace("\\","_")
		
		c_header.write('extern const char ${arrayname}[${fc.len}];\n')

		carray_begin := "const char ${arrayname}[${fc.len}]= {"
		carray_context := fc.map('0x$it.hex()').str().trim('[]').replace('\'','')
		carray_end := "};"
		c_src.write('$carray_begin$carray_context$carray_end')
		c_src.write('\n')
	}
	c_header.write('#endif //$macro_name\n')
	//write header
	os.write_file(header_filename,c_header.str())
	//write source
	os.write_file(source_filename,c_src.str())	
}