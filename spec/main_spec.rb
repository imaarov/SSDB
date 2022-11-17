describe 'database' do
    def run_script(commands)
      raw_output = nil
      IO.popen("./out/bin/SSDB", "r+") do |pipe|
        commands.each do |command|
          pipe.puts command
        end
  
        pipe.close_write
  
        # Read entire output
        raw_output = pipe.gets(nil)
      end
      raw_output.split("\n")
    end
  
    it 'inserts and retrieves a row' do
      result = run_script([
        "insert 1 user1 person1@example.com",
        "select",
        "!exit",
      ])
      expect(result).to match_array([
        "ssdb > Executed.",
        "ssdb > (1, user1, person1@example.com)",
        "Executed.",
        "ssdb > ",
      ])
    end
  end
  