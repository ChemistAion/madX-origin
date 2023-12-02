using Sharpmake;
using System;
using System.IO;

namespace Mockup
{
	////////////////////////////////////////////////////////////////////////////////

	[Fragment, Flags]
	public enum CustomMode
	{
		Development		= 1,
		Profile			= 2,
		Release			= 4
	}

	class CustomTarget : Sharpmake.ITarget
	{
		public Platform Platform = Platform.win64;
		public DevEnv DevEnv = DevEnv.vs2022;
		public BuildSystem BuildSystem = BuildSystem.MSBuild;

		public CustomMode Mode;

		public CustomTarget()
        {
        }
		
		public CustomTarget(CustomMode mode)
        {
			Mode = mode;
		}

		public override string Name
        {
			get { return Mode.ToString(); }
		}		
}

	////////////////////////////////////////////////////////////////////////////////

	class CustomProject : Sharpmake.Project
	{
		public CustomProject() : base(typeof(CustomTarget))
		{
			RootPath = @"[project.SharpmakeCsPath]\.build\[project.Name]";

			SourceFilesExcludeRegex.Add(@"\\___+");
			
			SourceFilesExtensions.Add(".ixx");
			SourceFilesCompileExtensions.Add(".ixx");			

			CustomTarget target = new CustomTarget(CustomMode.Development | CustomMode.Profile | CustomMode.Release);
			AddTargets(target);
			
			//	CustomProperties.Add("VcpkgEnabled", "true");
			//	CustomProperties.Add("VcpkgEnableManifest", "true");
			//	CustomProperties.Add("VcpkgTriplet", "x64-windows-static");
			//	CustomProperties.Add("VcpkgConfiguration", "Release");
		}
	
		//	[Configure]
		//	void Configure(Configuration conf, Target target)
		//	{
		//	ConfigureOptimization(conf, target);
		//	ConfigureDefines(conf, target);
		//	}
	
		//	public virtual void ConfigureOptimization(Configuration conf, Target target)
		//	{
		//	}
		//	public virtual void ConfigureDefines(Configuration conf, Target target)
		//	{
		//	}

		[Configure()]
		public virtual void Configure(Configuration conf, CustomTarget target)
		{
			conf.ProjectFileName = "[project.Name]_[target.DevEnv]_[target.Platform]";
			conf.ProjectPath = @"[project.RootPath]";

			//	conf.PrecompHeader = @"_[project.Name].hpp";
			//	conf.PrecompSource = @"_[project.Name].cpp";
			//	conf.PrecompSourceExcludeFolders.Add(@"[project.SharpmakeCsPath]\external");
			//	conf.PrecompSourceExcludeFolders.Add(@"[project.SharpmakeCsPath]\tools");
			conf.PrecompSourceExcludeExtension.Add(".ixx");

			conf.Options.Add(Options.Vc.General.WarningLevel.Level3);
			conf.Options.Add(Options.Vc.General.WindowsTargetPlatformVersion.v10_0_20348_0);
			//conf.Options.Add(Options.Vc.General.WindowsTargetPlatformVersion.Latest);
			conf.Options.Add(Options.Vc.General.PlatformToolset.v143);

			conf.Options.Add(Options.Vc.Compiler.CppLanguageStandard.CPP20);
			//conf.Options.Add(Options.Vc.Compiler.CppLanguageStandard.Latest);
			conf.Options.Add(Options.Vc.Compiler.Exceptions.Enable);
			conf.Options.Add(Options.Vc.Compiler.RTTI.Enable);
			conf.Options.Add(Options.Vc.Compiler.FloatingPointModel.Precise);
			conf.Options.Add(Options.Vc.Compiler.FunctionLevelLinking.Disable);
			conf.Options.Add(Options.Vc.Compiler.RuntimeLibrary.MultiThreaded);
			conf.Options.Add(Options.Vc.Compiler.EnhancedInstructionSet.AdvancedVectorExtensions2);
			conf.Options.Add(Options.Vc.Compiler.SupportJustMyCode.Yes);

			conf.Options.Add(Options.Vc.Linker.SubSystem.Windows);

			//conf.AdditionalCompilerOptions.Add(@"/bigobj");
			
			//modules
			conf.Options.Add(new Sharpmake.Options.Vc.Compiler.DisableSpecificWarnings("4005", "5106"));

			switch (target.Mode)
			{
				case CustomMode.Development:
					{
						conf.Options.Add(Options.Vc.General.DebugInformation.ProgramDatabase);
						
						conf.Options.Add(Options.Vc.Compiler.RuntimeLibrary.MultiThreadedDebug);
						conf.Options.Add(Options.Vc.Compiler.FavorSizeOrSpeed.Neither);
						conf.Options.Add(Options.Vc.Compiler.Optimization.Disable);
						conf.Options.Add(Options.Vc.Compiler.Inline.Disable);	

						//conf.Defines.Add("DEBUG");
						break;
					}
				case CustomMode.Profile:			
				case CustomMode.Release:
					{
						conf.Options.Add(Options.Vc.Compiler.RuntimeLibrary.MultiThreaded);
						conf.Options.Add(Options.Vc.Compiler.FavorSizeOrSpeed.FastCode);
						conf.Options.Add(Options.Vc.Compiler.Optimization.FullOptimization);
						conf.Options.Add(Options.Vc.Compiler.Inline.AnySuitable);

						// conf.Options.Add(Options.Vc.General.WholeProgramOptimization.LinkTime);
						break;
					}
			}

			conf.IncludePaths.Add(@"[project.SourceRootPath]");
			
			//	conf.IncludePrivatePaths.Add(@"[project.SharpmakeCsPath]\external");
			//	conf.IncludePrivatePaths.Add(@"[project.SourceRootPath]");
			//	conf.IncludePrivatePaths.Add(@"[project.SharpmakeCsPath]\source");
			
			//	conf.PrivateIncludePaths.Add();
			//	conf.LibraryPaths.Add();
			//	conf.LibraryFiles.Add();

			conf.VcxprojUserFile = new Project.Configuration.VcxprojUserFileSettings();
		}
	}

	////////////////////////////////////////////////////////////////////////////////

	class CustomLibrary : CustomProject
	{
		public CustomLibrary()
		{
			RootPath = @"[project.SharpmakeCsPath]\.build\[project.Name]";
		}

		public override void Configure(Configuration conf, CustomTarget target)
		{
			base.Configure(conf, target);
			conf.Output = Configuration.OutputType.Lib;
		}
	}

	////////////////////////////////////////////////////////////////////////////////

	class CustomProgram : CustomProject
	{
		public CustomProgram()
		{
			RootPath = @"[project.SharpmakeCsPath]\.build\[project.Name]";
		}
	}

	////////////////////////////////////////////////////////////////////////////////

	class CustomSolution : Sharpmake.Solution
	{
		public CustomSolution() : base(typeof(CustomTarget))
	    {
			CustomTarget target = new CustomTarget(CustomMode.Development | CustomMode.Profile | CustomMode.Release);
			AddTargets(target);
		}
	
		[Configure()]
		public virtual void Configure(Configuration conf, CustomTarget target)
	    {
			conf.SolutionFileName = "[solution.Name]_[target.DevEnv]_[target.Platform]";
			conf.SolutionPath = @"[solution.SharpmakeCsPath]\.build\";
		}
	}

	////////////////////////////////////////////////////////////////////////////////
}