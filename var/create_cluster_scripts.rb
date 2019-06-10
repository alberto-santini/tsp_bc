require 'fileutils'

homedir = '/homes/users/asantini'
binfdir = File.join(homedir, 'local', 'bin')
tspbdir = File.join(homedir, 'local', 'src', 'tsp_bc')
execdir = File.join(tspbdir, 'build')
instdir = File.join(tspbdir, 'instances')
libsdir = "#{File.join(homedir, 'local', 'lib')}:#{File.join(homedir, 'local', 'lib64')}"
enum_k = ARGV[0].to_i
proximity_n = ARGV[1].to_i

instances = ['gr17', 'gr21', 'gr24', 'fri26', 'bayg29', 'dantzig42', 'att48',
  'hk48', 'gr48', 'eil51', 'berlin52', 'brazil58', 'st70', 'pr76',
  'eil76', 'rat99', 'kroE100', 'kroD100', 'kroC100', 'kroB100',
  'kroA100', 'rd100', 'eil101', 'lin105', 'pr107', 'gr120', 'pr124',
  'bier127', 'ch130', 'pr136', 'pr144', 'kroB150', 'ch150',
  'kroA150', 'pr152', 'u159', 'brg180', 'rat195', 'd198', 'kroA200',
  'kroB200', 'ts225', 'tsp225', 'pr226', 'gil262', 'pr264', 'a280',
  'lin318', 'rd400', 'p654']

FileUtils.mkdir_p('cluster-scripts')

puts "Generating launchers for instance in #{instdir}..."

Dir.glob(File.join(instdir, '*.tsp')) do |instance|
  next unless instances.any?{|allowed| instance.include? allowed}

  inst_name = File.basename(instance, File.extname(instance))
  out_file = "#{enum_k}-#{proximity_n}-#{inst_name}.csv"

  script = <<~EOF
  #!/bin/bash
  #SBATCH --partition=normal
  #SBATCH --time=3:00:00
  #SBATCH --nodes=1
  #SBATCH --ntasks-per-node=1
  #SBATCH --cpus-per-task=1
  #SBATCH --mem-per-cpu=4096
  #SBATCH -o #{enum_k}-#{proximity_n}-#{inst_name}.txt
  #SBATCH -e err-#{enum_k}-#{proximity_n}-#{inst_name}.txt

  PATH=#{binfdir} LD_LIBRARY_PATH=#{libsdir} #{File.join(execdir, 'tsp_bc')} #{instance} #{enum_k} #{proximity_n} #{out_file}
  EOF

  File.write(File.join('cluster-scripts', "launch-#{enum_k}-#{proximity_n}-#{inst_name}.sh"), script)
end