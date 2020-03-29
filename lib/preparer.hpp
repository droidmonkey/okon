#pragma once

#include "btree_sorted_keys_inserter.hpp"
#include "fstream_wrapper.hpp"
#include "original_file_reader.hpp"
#include "sha1_utils.hpp"
#include "splitted_files.hpp"

#include <array>
#include <condition_variable>
#include <fstream>
#include <optional>
#include <string_view>

namespace okon {
constexpr auto k_intermediate_files_count{ 256u };

class preparer
{
public:
  enum class result
  {
    success,
    could_not_open_input_file,
    could_not_open_intermediate_files,
    could_not_open_output
  };

  explicit preparer(std::string_view input_file_path, std::string_view working_directory_path,
                    std::string_view output_file_path);

  result prepare();

private:
  void add_sha1_to_file(std::string_view sha1);

  void sort_files();
  void start_writing_sorted_files_thread();

  void write_sha1_buffer(unsigned buffer_index);

private:
  fstream_wrapper m_input_file_wrapper;
  original_file_reader<fstream_wrapper> m_input_reader;
  splitted_files m_intermediate_files;
  fstream_wrapper m_output_file_wrapper;
  btree_sorted_keys_inserter<fstream_wrapper> m_btree;
  std::vector<std::vector<sha1_t>> m_sha1_buffers;

  std::mutex m_processing_sorted_files_mtx;
  std::array<std::condition_variable, k_intermediate_files_count> m_sorted_files_cvs;
  std::array<bool, k_intermediate_files_count> m_sorted_files_ready_state;
  std::thread m_writing_sorted_files_thread;
};
}
