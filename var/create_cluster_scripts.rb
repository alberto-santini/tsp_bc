require 'fileutils'

homedir = '/homes/users/asantini'
binfdir = File.join(homedir, 'local', 'bin')
tspbdir = File.join(homedir, 'local', 'src', 'tsp_bc')
execdir = File.join(tspbdir, 'build')
instdir = File.join(tspbdir, 'instances')
libsdir = "#{File.join(homedir, 'local', 'lib')}:#{File.join(homedir, 'local', 'lib64')}"
enum_k = ARGV[0].to_i

FileUtils.mkdir_p('cluster-scripts')

puts "Generating launchers for instance in #{instdir}..."

Dir.glob(File.join(instdir, '*.tsp')) do |instance|
  inst_name = File.basename(instance, File.extname(instance))
  out_file = "#{enum_k}-#{inst_name}.csv"

  script = <<~EOF
  #!/bin/bash
  #SBATCH --partition=normal
  #SBATCH --time=3:00:00
  #SBATCH --nodes=1
  #SBATCH --ntasks-per-node=1
  #SBATCH --cpus-per-task=1
  #SBATCH --mem-per-cpu=4096
  #SBATCH -o #{enum_k}-#{inst_name}.txt
  #SBATCH -e err-#{enum_k}-#{inst_name}.txt

  PATH=#{binfdir} LD_LIBRARY_PATH=#{libsdir} #{File.join(execdir, 'tsp_bc')} #{instance} #{enum_k} #{out_file}
  EOF

  File.write(File.join('cluster-scripts', "launch-#{enum_k}-#{inst_name}.sh"), script)
end