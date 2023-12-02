#include "_tests.hpp"

#include "core_filesystem.hpp"

TEST_F(CoreFilesystem, EntryBasics)
{
	using namespace Filesystem;
	
	std::filesystem::path path1 = L"error";
	std::filesystem::path path2 = std::filesystem::current_path() / L"error";
	std::filesystem::path path3 = path_ / L"error";
	std::filesystem::path path4 = path_;
	
	Entry entry1(path1);
	Entry entry2(path2);
	Entry entry3(path3);
	Entry entry4(path4);
	
	Entry dummy;
	
	EXPECT_EQ(dummy.Touch(), EntryState::Unknown);
	EXPECT_EQ(entry1.Touch(), EntryState::Unknown);
	EXPECT_EQ(entry2.Touch(), EntryState::Unknown);
	EXPECT_EQ(entry3.Touch(), EntryState::Unknown);
	EXPECT_EQ(entry4.Touch(), EntryState::Valid);
}

TEST_F(CoreFilesystem, EntryDeleteAll)
{
	using namespace Filesystem;
	
	Entry dummy;
	
	Entry entry(path_);
	Entry entry1(dir1_);
	Entry entry11(entry11_);
	Entry entry22(entry22_);
		
	EXPECT_EQ(dummy.Touch(), EntryState::Unknown);
	EXPECT_EQ(entry.Touch(), EntryState::Valid);
	EXPECT_EQ(entry1.Touch(), EntryState::Valid);
	EXPECT_EQ(entry11.Touch(), EntryState::Valid);
	EXPECT_EQ(entry22.Touch(), EntryState::Valid);
		
	std::filesystem::remove_all(path_);
	
	EXPECT_EQ(dummy.Touch(), EntryState::Unknown);
	EXPECT_EQ(entry.Touch(), EntryState::Unknown);
	EXPECT_EQ(entry1.Touch(), EntryState::Unknown);
	EXPECT_EQ(entry11.Touch(), EntryState::Unknown);
	EXPECT_EQ(entry22.Touch(), EntryState::Unknown);
}

TEST_F(CoreFilesystem, EntryDeleteSome)
{
	using namespace Filesystem;
	
	Entry entry(path_);
	
	Entry entry1(dir1_);
	Entry entry2(dir2_);
	Entry entry3(dir3_);
	Entry entry11(entry11_);
	Entry entry12(entry12_);
	Entry entry22(entry22_);
	Entry entry23(entry23_);
	Entry entry33a(entry33_);
	Entry entry33b(entry33_);
	Entry entry33c(entry33_);
	
	EXPECT_EQ(entry1.Touch(), EntryState::Valid);
	EXPECT_EQ(entry2.Touch(), EntryState::Valid);
	EXPECT_EQ(entry3.Touch(), EntryState::Valid);
	
	EXPECT_EQ(entry11.Touch(), EntryState::Valid);
	EXPECT_EQ(entry12.Touch(), EntryState::Valid);
	
	std::filesystem::remove_all(dir1_);
	std::filesystem::remove_all(dir2_);
	
	EXPECT_EQ(entry1.Touch(), EntryState::Unknown);
	EXPECT_EQ(entry1.Touch(), EntryState::Unknown);
	
	EXPECT_EQ(entry2.Touch(), EntryState::Unknown);
	EXPECT_EQ(entry2.Touch(), EntryState::Unknown);
}

////////////////////////////////////////////////////////////////////////////////

TEST_F(CoreFilesystem, VolumeBasics)
{
	using namespace Filesystem;

	std::filesystem::path path;
	std::filesystem::path path1 = L"error";
	std::filesystem::path path2 = std::filesystem::current_path() / L"error";
	std::filesystem::path path3 = path_ / L"error";
	std::filesystem::path path4 = path_;

	Volume volume1(path1);
	Volume volume2(path2);
	Volume volume3(path3);
	Volume volume4(path4);

	Volume dummy;

	EXPECT_EQ(dummy.Touch(), GetElements(path));
	EXPECT_EQ(volume1.Touch(), GetElements(path1));
	EXPECT_EQ(volume2.Touch(), GetElements(path2));
	EXPECT_EQ(volume3.Touch(), GetElements(path3));
	EXPECT_EQ(volume4.Touch(), GetElements(path4));
}