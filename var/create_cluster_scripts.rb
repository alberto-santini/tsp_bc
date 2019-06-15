require 'fileutils'

homedir = '/homes/users/asantini'
binfdir = File.join(homedir, 'local', 'bin')
tspbdir = File.join(homedir, 'local', 'src', 'tsp_bc')
execdir = File.join(tspbdir, 'build')
instdir = File.join(tspbdir, 'instances')
libsdir = "#{File.join(homedir, 'local', 'lib')}:#{File.join(homedir, 'local', 'lib64')}"
enum_k = ARGV[0].to_i
proximity_n = ARGV[1].to_i

FileUtils.mkdir_p('cluster-scripts')

puts "Generating launchers for instance in #{instdir}..."

Dir.glob(File.join(instdir, '*.tsp')) do |instance|
  inst_name = File.basename(instance, File.extname(instance))
  inst_size = inst_name.gsub(/\D/, '').to_i
  memory = (inst_size < 1000 ? 4096 : 8192)
  out_file = "#{enum_k}-#{proximity_n}-#{inst_name}.csv"

  script = <<~EOF
  #!/bin/bash
  #SBATCH --partition=normal
  #SBATCH --time=3:00:00
  #SBATCH --nodes=1
  #SBATCH --ntasks-per-node=1
  #SBATCH --cpus-per-task=1
  #SBATCH --mem-per-cpu=#{memory}
  #SBATCH -o #{enum_k}-#{proximity_n}-#{inst_name}.txt
  #SBATCH -e err-#{enum_k}-#{proximity_n}-#{inst_name}.txt

  PATH=#{binfdir} LD_LIBRARY_PATH=#{libsdir} #{File.join(execdir, 'tsp_bc')} #{instance} #{enum_k} #{proximity_n} #{out_file}
  EOF

  File.write(File.join('cluster-scripts', "launch-#{enum_k}-#{proximity_n}-#{inst_name}.sh"), script)
end