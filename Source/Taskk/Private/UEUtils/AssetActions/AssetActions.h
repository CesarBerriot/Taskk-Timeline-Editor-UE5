#pragma once

namespace UEUtils::AssetActions
{
	/// SaveNavigationMesh() Result
	enum class ESaveError
	{
		/// All operations were successful
		Success,
		/// The last step (calling UE's UPackage::SavePackage()) failed
		ErrorSavePackageFail,

		// Read-Only removal errors
		/// File attributes for the given asset couldn't be retrieved through the Win32 API (using GetFileAttributes())
		ErrorGetFileAttributesFail,
		/// File attributes for the given asset couldn't be modified through the Win32 API (using SetFileAttributes()), access to the file might've been denied
		ErrorReadOnlyRemovalFail,

		// Source control error
		/// Source Control is enabled, though unreachable (might be a problem with VCS server connection)
		ErrorSourceControlEnabledThoughUnavailable,
		/// Source Control checkout failed (maybe the file no longer exists ?)
		ErrorCheckOutFail,
	};
	/// Saves the given asset's package. Check out the documentation of **ESaveError** for detailed information about the results.
	ESaveError SaveAsset(UObject * Asset);
};
