using Sharpmake;
using System;
//using System.Data;
//using System.Diagnostics;

[module: Sharpmake.Include("custom.sharpmake.cs")]

//	https://vector-of-bool.github.io/2019/03/10/modules-1.html
//	https://devblogs.microsoft.com/cppblog/moving-a-project-to-cpp-named-modules/
//  https://learn.microsoft.com/en-us/cpp/build/walkthrough-import-stl-header-units

namespace Mockup
{
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
	//MODULES external
	
    [Sharpmake.Generate]
	class stl : CustomLibrary
    {
		public stl()
		{
			SourceRootPath = @"[project.SharpmakeCsPath]\modules\stl";			
			//CustomProperties.Add("ScanSourceForModuleDependencies", "true");
		}
		
		public override void Configure(Configuration conf, CustomTarget target)
		{
			base.Configure(conf, target);
			conf.SolutionFolder = "microsoft";
		}
    }	
	
    [Sharpmake.Generate]
	class windows : CustomLibrary
    {
		public windows()
		{
			SourceRootPath = @"[project.SharpmakeCsPath]\modules\windows";			
			//CustomProperties.Add("ScanSourceForModuleDependencies", "true");			
		}
		
		public override void Configure(Configuration conf, CustomTarget target)
		{
			base.Configure(conf, target);
			conf.SolutionFolder = "microsoft";			
			
			conf.AddPrivateDependency<stl>(target);			
			
			conf.LibraryFiles.Add("runtimeobject.lib");
			conf.LibraryFiles.Add("comsuppw.lib");
			conf.LibraryFiles.Add("onecore.lib");
			//	conf.LibraryFiles.Add("dbghelp.lib");
			//	conf.LibraryFiles.Add("shlwapi.lib");			
		}
    }
	
    [Sharpmake.Generate]
	class directx : CustomLibrary
    {
		public directx()
		{
			SourceRootPath = @"[project.SharpmakeCsPath]\modules\directx";			
			//CustomProperties.Add("ScanSourceForModuleDependencies", "true");	
			
			////	SourceFiles.Add(@"[project.SharpmakeCsPath]\external\xheaders\src\dxguids.cpp");
			////	SourceFiles.Add(@"[project.SharpmakeCsPath]\external\xheaders\include\wsl\winadapter.h");			
			////	SourceFiles.Add(@"[project.SharpmakeCsPath]\external\xheaders\include\directx\dxcore.h");			
			////	SourceFiles.Add(@"[project.SharpmakeCsPath]\external\xheaders\include\directx\d3d12.h");			
			////	SourceFiles.Add(@"[project.SharpmakeCsPath]\external\xheaders\include\directx\d3d12video.h");
			////	SourceFiles.Add(@"[project.SharpmakeCsPath]\external\xheaders\include\directx\d3dx12.h");
			////	
			////	SourceFiles.Add(@"[project.SharpmakeCsPath]\external\xcompiler\include\dxc\support\dxcapi.use.h");		
			////	
			////	SourceFiles.Add(@"[project.SharpmakeCsPath]\external\dxc\inc\d3d12shader.h");			
			////	SourceFiles.Add(@"[project.SharpmakeCsPath]\external\dxc\inc\dxcapi.h");
			
		}
		
		public override void Configure(Configuration conf, CustomTarget target)
		{
			base.Configure(conf, target);
			conf.SolutionFolder = "microsoft";			
			
			//conf.AddPrivateDependency<stl>(target);
			//conf.AddPrivateDependency<windows>(target);
			
			//conf.LibraryPaths.Add(@"\external\dxc\lib\x64");
			conf.LibraryPaths.Add(@"\packages\dxc\lib\x64");
			
			////	conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\external\xcompiler\include");	
			////	conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\external\xheaders\include");
			////	conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\external\dxc\inc");	
			////	conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\external\xheaders\include\directx");
			
			//conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\external\agilitysdk\build\native\include");
			conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\packages\agilitysdk\build\native\include");
			//conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\external\agilitysdk\build\native\include\d3dx12");
			conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\packages\agilitysdk\build\native\include\d3dx12");


			//conf.TargetCopyFiles.Add(@"\external\dxc\bin\x64\dxcompiler.dll");
			conf.TargetCopyFiles.Add(@"\packages\dxc\bin\x64\dxcompiler.dll");
			//conf.TargetCopyFiles.Add(@"\external\dxc\bin\x64\dxil.dll");
			conf.TargetCopyFiles.Add(@"\packages\dxc\bin\x64\dxil.dll");
			
			//conf.TargetCopyFiles.Add(@"\external\agilitysdk\build\native\bin\x64\d3d12core.dll");
			//conf.TargetCopyFiles.Add(@"\external\agilitysdk\build\native\bin\x64\d3d12sdklayers.dll");
			
			//conf.LibraryFiles.Add("dxgi.lib");
			//conf.LibraryFiles.Add("d3d12.lib");
			//conf.LibraryFiles.Add("dxguid.lib");
			//conf.LibraryFiles.Add("dxcompiler.lib");
		}
    }
	
	[Sharpmake.Generate]
	class xmath : CustomLibrary
	{
		public xmath()
		{
			SourceRootPath = @"[project.SharpmakeCsPath]\modules\xmath";			
			//CustomProperties.Add("ScanSourceForModuleDependencies", "true");			
		}
		
		public override void Configure(Configuration conf, CustomTarget target)
		{
			base.Configure(conf, target);
			conf.SolutionFolder = "microsoft";			
			
			conf.AddPrivateDependency<stl>(target);
			conf.AddPrivateDependency<windows>(target);			
			conf.AddPublicDependency<directx>(target);
		}
	}		
	
	[Sharpmake.Generate]
	class imgui : CustomLibrary
	{
		public imgui()
		{
			SourceRootPath = @"[project.SharpmakeCsPath]\modules\gui\imgui";			
			//CustomProperties.Add("ScanSourceForModuleDependencies", "true");			
			
			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imconfig.h");
			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imgui_internal.h");
			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imstb_rectpack.h");
			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imstb_textedit.h");
			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imstb_truetype.h");
			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imgui.h");
			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imgui.cpp");
			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imgui_draw.cpp");
			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imgui_widgets.cpp");
			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imgui_tables.cpp");
			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\backends\imgui_impl_win32.h");
			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\backends\imgui_impl_win32.cpp");
			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\backends\imgui_impl_dx12.h");
			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\backends\imgui_impl_dx12.cpp");
			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imgui_demo.cpp");			
		}
		
		public override void Configure(Configuration conf, CustomTarget target)
		{
			base.Configure(conf, target);
			conf.SolutionFolder = "gui";			
			
			conf.AddPrivateDependency<windows>(target);
			conf.AddPrivateDependency<directx>(target);
				
			conf.IncludePrivatePaths.Add(@"[project.SharpmakeCsPath]\external\imgui");
		}
	}	
	
	[Sharpmake.Generate]
	class cista : CustomLibrary
    {
		public cista()
		{
			SourceRootPath = @"[project.SharpmakeCsPath]\modules\utils\cista";			
			//CustomProperties.Add("ScanSourceForModuleDependencies", "true");
		}
		
		public override void Configure(Configuration conf, CustomTarget target)
		{
			base.Configure(conf, target);
			conf.SolutionFolder = "utils";
			
			conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\external\cista\include");	
		}
    }	
	
	[Sharpmake.Generate]
	class simdjson : CustomLibrary
    {
		public simdjson()
		{
			SourceRootPath = @"[project.SharpmakeCsPath]\modules\utils\simdjson";			
			//CustomProperties.Add("ScanSourceForModuleDependencies", "true");
			
			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\simdjson\src\simdjson.cpp");
			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\simdjson\include\simdjson.h");			
		}
		
		public override void Configure(Configuration conf, CustomTarget target)
		{
			base.Configure(conf, target);
			conf.SolutionFolder = "utils";
			
			conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\external\simdjson\include");
			conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\external\simdjson\src");			
		}
    }		
	
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
	//MODULES internal

    [Sharpmake.Generate]
	class core : CustomLibrary
    {
		public core()
		{
			SourceRootPath = @"[project.SharpmakeCsPath]\source\core";
			//CustomProperties.Add("ScanSourceForModuleDependencies", "true");
		}
		
		public override void Configure(Configuration conf, CustomTarget target)
		{
			base.Configure(conf, target);
			conf.SolutionFolder = "engine";		
			
			conf.AddPrivateDependency<stl>(target);
			conf.AddPrivateDependency<windows>(target);			
		}
    }
	
	[Sharpmake.Generate]
	class backend : CustomLibrary
	{
		public backend()
		{
			SourceRootPath = @"[project.SharpmakeCsPath]\source\backend";
		}
	
		public override void Configure(Configuration conf, CustomTarget target)
		{
			base.Configure(conf, target);
			conf.SolutionFolder = "engine";			
	
			conf.AddPublicDependency<directx>(target);
			conf.AddPrivateDependency<stl>(target);
			//conf.AddPrivateDependency<windows>(target);
			conf.AddPrivateDependency<core>(target);
		}
	}
	
	[Sharpmake.Generate]
	class frontend : CustomLibrary
	{
		public frontend()
		{
			SourceRootPath = @"[project.SharpmakeCsPath]\source\frontend";
		}
	
		public override void Configure(Configuration conf, CustomTarget target)
		{
			base.Configure(conf, target);
			conf.SolutionFolder = "engine";				
	
			conf.AddPrivateDependency<stl>(target);
			conf.AddPrivateDependency<windows>(target);
			conf.AddPrivateDependency<core>(target);
		}
	}	
	
	[Sharpmake.Generate]
	class framegraph : CustomLibrary
	{
		public framegraph()
		{
			SourceRootPath = @"[project.SharpmakeCsPath]\source\framegraph";
		}
	
		public override void Configure(Configuration conf, CustomTarget target)
		{
			base.Configure(conf, target);
			conf.SolutionFolder = "engine";				
	
			conf.AddPrivateDependency<stl>(target);
			conf.AddPrivateDependency<core>(target);
		}
	}		

	[Sharpmake.Generate]
	class meta : CustomLibrary
	{
		public meta()
		{
			SourceRootPath = @"[project.SharpmakeCsPath]\source\meta";
		}
	
		public override void Configure(Configuration conf, CustomTarget target)
		{
			base.Configure(conf, target);
			conf.SolutionFolder = "engine";				
	
			conf.AddPrivateDependency<stl>(target);
			conf.AddPrivateDependency<core>(target);
			conf.AddPrivateDependency<simdjson>(target);
			conf.AddPrivateDependency<cista>(target);
		}
	}	
	
	[Sharpmake.Generate]
	class project : CustomLibrary
	{
		public project()
		{
			SourceRootPath = @"[project.SharpmakeCsPath]\source\project";
		}
	
		public override void Configure(Configuration conf, CustomTarget target)
		{
			base.Configure(conf, target);
			conf.SolutionFolder = "engine";			
	
			conf.AddPrivateDependency<stl>(target);
			conf.AddPrivateDependency<core>(target);
			conf.AddPrivateDependency<meta>(target);
		}
	}
		
	[Sharpmake.Generate]
	class platform : CustomLibrary
	{
		public platform()
		{
			SourceRootPath = @"[project.SharpmakeCsPath]\source\platform";		
		}
	
		public override void Configure(Configuration conf, CustomTarget target)
		{
			base.Configure(conf, target);
			conf.SolutionFolder = "engine";					
	
			conf.AddPrivateDependency<stl>(target);
			conf.AddPrivateDependency<windows>(target);
			conf.AddPrivateDependency<core>(target);
		}
	}		

	[Sharpmake.Generate]
	class editor : CustomLibrary
	{
		public editor()
		{
			SourceRootPath = @"[project.SharpmakeCsPath]\source\editor";
		}
	
		public override void Configure(Configuration conf, CustomTarget target)
		{
			base.Configure(conf, target);
	
			conf.AddPrivateDependency<stl>(target);
			conf.AddPrivateDependency<windows>(target);
			conf.AddPrivateDependency<core>(target);
			conf.AddPrivateDependency<directx>(target);
			conf.AddPrivateDependency<xmath>(target);
			conf.AddPrivateDependency<backend>(target);
			conf.AddPrivateDependency<frontend>(target);
			conf.AddPrivateDependency<framegraph>(target);			
			conf.AddPrivateDependency<platform>(target);
			conf.AddPrivateDependency<imgui>(target);
			conf.AddPrivateDependency<project>(target);
		}
	}	

	////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
	//project madX components
	
	[Sharpmake.Generate]
	class root : CustomProgram
	{
		public root()
		{
			SourceRootPath = @"[project.SharpmakeCsPath]\source\root";
			ResourceFiles.Add(@"[project.SharpmakeCsPath]\source\root\resources.rc");		
		}
		
		public override void Configure(Configuration conf, CustomTarget target)
		{
			base.Configure(conf, target);
	
			conf.AddPrivateDependency<stl>(target);
			conf.AddPrivateDependency<windows>(target);
			conf.AddPrivateDependency<core>(target);	
			conf.AddPrivateDependency<directx>(target);			
			conf.AddPrivateDependency<backend>(target);
			conf.AddPrivateDependency<frontend>(target);		
			conf.AddPrivateDependency<platform>(target);			
			conf.AddPrivateDependency<imgui>(target);
			conf.AddPrivateDependency<editor>(target);
					
			conf.VcxprojUserFile.LocalDebuggerWorkingDirectory = @"[project.SharpmakeCsPath]\workdir";
		}
	}	

	[Sharpmake.Generate]
	class madX : CustomSolution
    {
		public madX()
		{
			//	ExtraItems["extras"] = new Strings
			//	{
			//	    @"[solution.SharpmakeCsPath]\.clang-format",
			//	};
		}
			
		public override void Configure(Configuration conf, CustomTarget target)
        {
			base.Configure(conf, target);

			conf.AddProject<root>(target);	
			//	conf.AddProject<tests>(target);
			//	conf.AddProject<benchmarks>(target);
        }
	}

	public static class Bootstrap
	{
		[Sharpmake.Main]
		public static void SharpmakeMain(Sharpmake.Arguments arguments)
		{
			arguments.Generate<madX>();
		}
	}
}






	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////

	//	[Sharpmake.Generate]
	//	class google_test : CustomLibrary
	//	{
	//		public google_test()
	//		{
	//			SourceRootPath = @"[project.SharpmakeCsPath]\tools\googletest\googletest";
	//			SourceFilesExcludeRegex.Add(@"\\cmake");
	//			SourceFilesExcludeRegex.Add(@"\\samples");
	//			SourceFilesExcludeRegex.Add(@"\\scripts");			
	//			SourceFilesExcludeRegex.Add(@"\\test");
	//			SourceFilesExcludeRegex.Add(@"gtest-all.cc");
	//		}
	//	
	//		public override void Configure(Configuration conf, CustomTarget target)
	//		{
	//			base.Configure(conf, target);
	//			
	//			conf.Defines.Add("GTEST_ENABLE_CATCH_EXCEPTIONS_=1");
	//			conf.Defines.Add("gtest_build_tests=OFF");
	//			conf.Defines.Add("gtest_build_samples=OFF");
	//			
	//			conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\tools\googletest\googletest\include");
	//		}
	//	}
		
	//	[Sharpmake.Generate]
	//	class google_mock : CustomLibrary
	//	{
	//		public google_mock()
	//		{
	//			SourceRootPath = @"[project.SharpmakeCsPath]\tools\googletest\googlemock";
	//			SourceFilesExcludeRegex.Add(@"\\cmake");
	//			SourceFilesExcludeRegex.Add(@"\\scripts");
	//			SourceFilesExcludeRegex.Add(@"\\test");
	//		}
	//	
	//		public override void Configure(Configuration conf, CustomTarget target)
	//		{
	//			base.Configure(conf, target);
	//					
	//			conf.AddPrivateDependency<google_test>(target);
	//			
	//			conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\tools\googletest\googlemock\include");
	//		}
	//	}
		
	//	[Sharpmake.Generate]
	//	class google_benchmark : CustomLibrary
	//	{
	//		public google_benchmark()
	//		{
	//			SourceRootPath = @"[project.SharpmakeCsPath]\tools\benchmark";
	//			SourceFilesExcludeRegex.Add(@"\\bindings");
	//			SourceFilesExcludeRegex.Add(@"\\cmake");
	//			SourceFilesExcludeRegex.Add(@"\\conan");
	//			SourceFilesExcludeRegex.Add(@"\\test");		
	//			////SourceFilesExcludeRegex.Add(@"\\tools");
	//		}
	//	
	//		public override void Configure(Configuration conf, CustomTarget target)
	//		{
	//			base.Configure(conf, target);
	//			
	//			conf.AddPrivateDependency<google_test>(target);
	//			conf.AddPrivateDependency<google_mock>(target);
	//			conf.AddPrivateDependency<core>(target);
	//			
	//			conf.Defines.Add("BENCHMARK_ENABLE_EXCEPTIONS=1");
	//			conf.Defines.Add("BENCHMARK_USE_LIBCXX=0");
	//								
	//			conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\tools\benchmark\include");
	//		}
	//	}
	
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	
	//////	[Sharpmake.Generate]
	//////	class xmath : CustomLibrary
	//////	{
	//////		public xmath()
	//////		{
	//////			SourceRootPath = @"[project.SharpmakeCsPath]\modules\directx-math";			
	//////			CustomProperties.Add("ScanSourceForModuleDependencies", "true");			
	//////		}
	//////		
	//////		public override void Configure(Configuration conf, CustomTarget target)
	//////		{
	//////			base.Configure(conf, target);
	//////			
	//////			conf.AddPublicDependency<stl>(target);
	//////			conf.AddPublicDependency<windows>(target);			
	//////		}
	//////	}			

	//////	[Sharpmake.Generate]
	//////	class xtex : CustomLibrary
	//////	{
	//////		public xtex()
	//////		{
	//////			SourceRootPath = @"[project.SharpmakeCsPath]\external\xtex\directxtex";
	//////			SourceFilesExcludeRegex.Add(@"\\bcdirectcompute.h");				
	//////			SourceFilesExcludeRegex.Add(@"\\bcdirectcompute.cpp");				
	//////			SourceFilesExcludeRegex.Add(@"\\directxtexcompressgpu.cpp");						
	//////			SourceFilesExcludeRegex.Add(@"\\directxtexd3d11.cpp");					
	//////		}
	//////		
	//////		public override void Configure(Configuration conf, CustomTarget target)
	//////		{
	//////			base.Configure(conf, target);
	//////			
	//////			conf.AddPrivateDependency<core>(target);
	//////			
	//////			//conf.IncludePrivatePaths.Add(@"[project.SharpmakeCsPath]\external\xtex\directxtex");
	//////		}
	//////	}
	
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////	
	
	//	[Sharpmake.Generate]
	//	class tests : CustomProgram
	//	{
	//		public tests()
	//		{
	//			SourceRootPath = @"[project.SharpmakeCsPath]\workbench\tests";
	//		}
	//		
	//		public override void Configure(Configuration conf, CustomTarget target)
	//		{
	//			base.Configure(conf, target);
	//			
	//			conf.Options.Add(Options.Vc.Linker.SubSystem.Console);
	//			
	//			conf.AddPrivateDependency<core>(target);
	//			conf.AddPrivateDependency<ecs>(target);			
	//			//conf.AddPrivateDependency<root>(target);
	//			//conf.AddPrivateDependency<gltf>(target);
	//			//conf.AddPrivateDependency<mockup>(target);
	//			
	//			conf.AddPrivateDependency<google_test>(target);
	//			
	//			conf.VcxprojUserFile.LocalDebuggerWorkingDirectory = @"[project.SharpmakeCsPath]\workdir";
	//		}
	//	}
		
	//	[Sharpmake.Generate]
	//	class benchmarks : CustomProgram
	//	{
	//		public benchmarks()
	//		{
	//			SourceRootPath = @"[project.SharpmakeCsPath]\workbench\benchmarks";
	//		}
	//		
	//		public override void Configure(Configuration conf, CustomTarget target)
	//		{
	//			base.Configure(conf, target);
	//			
	//			conf.Options.Add(Options.Vc.Linker.SubSystem.Console);
	//			
	//			conf.AddPrivateDependency<core>(target);
	//			conf.AddPrivateDependency<google_benchmark>(target);
	//			
	//			conf.VcxprojUserFile.LocalDebuggerWorkingDirectory = @"[project.SharpmakeCsPath]\workdir";
	//		}
	//	}		
	
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
		
	//////	[Sharpmake.Generate]
	//////	class editor : CustomProgram
	//////	{
	//////		public editor()
	//////		{
	//////			SourceRootPath = @"[project.SharpmakeCsPath]\source\editor";
	//////			
	//////			ResourceFiles.Add(@"[project.SharpmakeCsPath]\source\editor\backend\resources.rc");
	//////		
	//////			//imgui
	//////			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imconfig.h");
	//////			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imgui_internal.h");
	//////			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imstb_rectpack.h");
	//////			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imstb_textedit.h");
	//////			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imstb_truetype.h");
	//////			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imgui.h");
	//////			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imgui.cpp");
	//////			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imgui_draw.cpp");
	//////			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imgui_widgets.cpp");
	//////			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imgui_tables.cpp");
	//////			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\backends\imgui_impl_win32.h");
	//////			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\backends\imgui_impl_win32.cpp");
	//////			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imgui_demo.cpp");
	//////			
	//////			//im3d
	//////			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\im3d\im3d_config.h");
	//////			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\im3d\im3d_math.h");			
	//////			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\im3d\im3d.h");
	//////			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\im3d\im3d.cpp");
	//////			
	//////			//imsequencer
	//////			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imguizmo\imcurveedit.h");
	//////			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imguizmo\imcurveedit.cpp");
	//////			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imguizmo\imsequencer.h");
	//////			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imguizmo\imsequencer.cpp");
	//////			
	//////			//	//imnodes
	//////			//	SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imnodes\imnodes.h");
	//////			//	SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imnodes\imnodes.cpp");
	//////			//	SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imnodes\imnodesez.h");
	//////			//	SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imnodes\imnodesez.cpp");			
	//////			
	//////			//	//imtextedit
	//////			//	SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imtextedit\imtextedit.h");
	//////			//	SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imtextedit\imtextedit.cpp");		
	//////			//	//////SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imtextedit\SPIRVParser.h");				
	//////			//	//////SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imtextedit\SPIRVParser.cpp");
	//////			//	//////SourceFiles.Add(@"[project.SharpmakeCsPath]\external\spvgentwo\lib\include\spvgentwo\spv.h");		
	//////		}
	//////	
	//////		public override void Configure(Configuration conf, CustomTarget target)
	//////		{
	//////			base.Configure(conf, target);
	//////	
	//////			conf.AddPrivateDependency<core>(target);
	//////			conf.AddPrivateDependency<root>(target);		
	//////			conf.AddPrivateDependency<engine>(target);
	//////			conf.AddPrivateDependency<meta>(target);
	//////			
	//////			conf.IncludePrivatePaths.Add(@"[project.SharpmakeCsPath]\external\imgui");
	//////			conf.IncludePrivatePaths.Add(@"[project.SharpmakeCsPath]\external\im3d");
	//////			conf.IncludePrivatePaths.Add(@"[project.SharpmakeCsPath]\external\imguizmo");
	//////			//conf.IncludePrivatePaths.Add(@"[project.SharpmakeCsPath]\external\imnodes");
	//////			conf.IncludePrivatePaths.Add(@"[project.SharpmakeCsPath]\external\imtextedit");
	//////			//conf.IncludePrivatePaths.Add(@"[project.SharpmakeCsPath]\external\spvgentwo\lib\include");
	//////			//conf.IncludePrivatePaths.Add(@"[project.SharpmakeCsPath]\external\spvgentwo\lib\include\spvgentwo");
	//////			
	//////			conf.VcxprojUserFile.LocalDebuggerWorkingDirectory = @"[project.SharpmakeCsPath]\workdir";
	//////			
	//////			//platform
	//////			conf.LibraryFiles.Add("dbghelp.lib");
	//////			conf.LibraryFiles.Add("shlwapi.lib");
	//////			conf.LibraryFiles.Add("runtimeobject.lib");
	//////		}
	//////	}
	
	
	//	[Sharpmake.Generate]
	//	class ecs : CustomLibrary
	//	{
	//		public ecs()
	//		{
	//			SourceRootPath = @"[project.SharpmakeCsPath]\source\ecs";
	//		}
	//		
	//		public override void Configure(Configuration conf, CustomTarget target)
	//		{
	//			base.Configure(conf, target);
	//			
	//			conf.AddPrivateDependency<core>(target);
	//		}
	//	}

	//////	[Sharpmake.Generate]
	//////	class meta : CustomLibrary
	//////	{
	//////		public meta()
	//////		{
	//////			SourceRootPath = @"[project.SharpmakeCsPath]\source\meta";
	//////			
	//////			//simdjson
	//////			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\simdjson\src\simdjson.cpp");
	//////			//SourceFiles.Add(@"[project.SharpmakeCsPath]\external\simdjson\include\simdjson.h");
	//////			
	//////			//imgui
	//////			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imconfig.h");
	//////			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imgui_internal.h");
	//////			//SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imstb_rectpack.h");
	//////			//SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imstb_textedit.h");
	//////			//SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imstb_truetype.h");
	//////			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imgui.h");
	//////			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imgui.cpp");
	//////			//SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imgui_draw.cpp");
	//////			//SourceFiles.Add(@"[project.SharpmakeCsPath]\external\imgui\imgui_widgets.cpp");
	//////		}
	//////		
	//////		public override void Configure(Configuration conf, CustomTarget target)
	//////		{
	//////			base.Configure(conf, target);
	//////	
	//////			conf.AddPrivateDependency<core>(target);
	//////			conf.AddPrivateDependency<engine>(target);
	//////			
	//////			//simdjson
	//////			conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\external\simdjson\include");
	//////			conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\external\simdjson\src");
	//////			
	//////			//imgui
	//////			conf.IncludePrivatePaths.Add(@"[project.SharpmakeCsPath]\external\imgui");
	//////			
	//////			//cista
	//////			conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\external\cista\include");			
	//////		}
	//////	}	
	
	//////	[Sharpmake.Generate]
	//////	class root : CustomLibrary
	//////	{
	//////		public root()
	//////		{
	//////			SourceRootPath = @"[project.SharpmakeCsPath]\source\root";
	//////			
	//////			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\xtex\directxtex\directxtex.h");			
	//////			SourceFiles.Add(@"[project.SharpmakeCsPath]\external\xtex\directxtex\directxtex.inl");			
	//////		}
	//////		
	//////		public override void Configure(Configuration conf, CustomTarget target)
	//////		{
	//////			base.Configure(conf, target);
	//////	
	//////			conf.AddPublicDependency<core>(target);
	//////			//conf.AddPrivateDependency<ecs>(target);
	//////			conf.AddPrivateDependency<meta>(target);
	//////			conf.AddPrivateDependency<xtex>(target);
	//////			conf.AddPrivateDependency<engine>(target);
	//////			
	//////			conf.IncludePrivatePaths.Add(@"[project.SharpmakeCsPath]\external\xtex\directxtex");
	//////		}
	//////	}	