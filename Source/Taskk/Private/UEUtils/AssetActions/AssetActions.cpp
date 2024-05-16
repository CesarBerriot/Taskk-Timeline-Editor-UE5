#include "AssetActions.h"

#include "SourceControlHelpers.h"
#include "Windows.h"

namespace UEUtils::AssetActions
{
	ESaveError SaveAsset(UObject * Asset)
	{
		UPackage * Package = Asset->GetPackage();
		// Acquire file path
		FString PackageFileName = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
		PackageFileName = FPaths::ConvertRelativePathToFull(PackageFileName);
		check(FPaths::ValidatePath(PackageFileName))
		// If the file exists, make sure it's writable
		if(FPaths::FileExists(PackageFileName))
		{
			// If source control is enabled, let it do its thing, else make the file writable
			if(USourceControlHelpers::IsEnabled())
			{
				if(!USourceControlHelpers::IsAvailable())
					return ESaveError::ErrorSourceControlEnabledThoughUnavailable;
				if(!USourceControlHelpers::CheckOutFile(PackageFileName))
					return ESaveError::ErrorCheckOutFail;
			}
			else
			{
				DWORD FileAttributes = GetFileAttributesW(*PackageFileName);
				if(FileAttributes == INVALID_FILE_ATTRIBUTES)
					return ESaveError::ErrorGetFileAttributesFail;
				if(FileAttributes & FILE_ATTRIBUTE_READONLY)
				{
					BOOL Success = SetFileAttributesW(*PackageFileName, FileAttributes & ~FILE_ATTRIBUTE_READONLY);
					if(!Success)
						return ESaveError::ErrorReadOnlyRemovalFail;
				}
			}
		}
		bool Success = UPackage::SavePackage(Package, Asset, Asset->GetFlags(), *PackageFileName);
		return Success ? ESaveError::Success : ESaveError::ErrorSavePackageFail;
	}
}
