#include <glib.h>
#include <glib-object.h>

/* BpG-skip */
#ifndef BD_LVM_API
#define BD_LVM_API

#define BD_LVM_TYPE_PVDATA (bd_lvm_pvdata_get_type ())
GType bd_lvm_pvdata_get_type();

typedef struct BDLVMPVdata {
    gchar *pv_name;
    gchar *pv_uuid;
    guint64 pe_start;
    gchar *vg_name;
    gchar *vg_uuid;
    guint64 vg_size;
    guint64 vg_free;
    guint64 vg_extent_size;
    guint64 vg_extent_count;
    guint64 vg_free_count;
    guint64 vg_pv_count;
} BDLVMPVdata;

/**
 * bd_lvm_pvdata_copy: (skip)
 *
 * Creates a new copy of @data.
 */
BDLVMPVdata* bd_lvm_pvdata_copy (BDLVMPVdata *data) {
    BDLVMPVdata *new_data = g_new (BDLVMPVdata, 1);

    new_data->pv_name = g_strdup (data->pv_name);
    new_data->pv_uuid = g_strdup (data->pv_uuid);
    new_data->pe_start = data->pe_start;
    new_data->vg_name = g_strdup (data->vg_name);
    new_data->vg_size = data->vg_size;
    new_data->vg_free = data->vg_free;
    new_data->vg_extent_size = data->vg_extent_size;
    new_data->vg_extent_count = data->vg_extent_count;
    new_data->vg_free_count = data->vg_free_count;
    new_data->vg_pv_count = data->vg_pv_count;

    return new_data;
}

/**
 * bd_lvm_pvdata_free: (skip)
 *
 * Frees @data.
 */
void bd_lvm_pvdata_free (BDLVMPVdata *data) {
    g_free (data->pv_name);
    g_free (data->pv_uuid);
    g_free (data->vg_name);
    g_free (data);
}

GType bd_lvm_pvdata_get_type () {
    static GType type = 0;

    if (G_UNLIKELY(type == 0)) {
        type = g_boxed_type_register_static("BDLVMPVdata",
                                            (GBoxedCopyFunc) bd_lvm_pvdata_copy,
                                            (GBoxedFreeFunc) bd_lvm_pvdata_free);
    }

    return type;
}
/* BpG-skip-end */

/**
 * bd_lvm_is_supported_pe_size:
 * @size: size (in bytes) to test
 *
 * Returns: whether the given size is supported physical extent size or not
 */
gboolean bd_lvm_is_supported_pe_size (guint64 size);

/**
 * bd_lvm_get_supported_pe_sizes:
 *
 * Returns: (transfer full) (array zero-terminated=1): list of supported PE sizes
 */
guint64 *bd_lvm_get_supported_pe_sizes ();

/**
 * bd_lvm_get_max_lv_size:
 *
 * Returns: maximum LV size in bytes
 */
guint64 bd_lvm_get_max_lv_size ();

/**
 * bd_lvm_round_size_to_pe:
 * @size: size to be rounded
 * @pe_size: physical extent (PE) size or 0 to use the default
 * @roundup: whether to round up or down (ceil or floor)
 *
 * Returns: @size rounded to @pe_size according to the @roundup
 *
 * Rounds given @size up/down to a multiple of @pe_size according to the value of
 * the @roundup parameter.
 */
guint64 bd_lvm_round_size_to_pe (guint64 size, guint64 pe_size, gboolean roundup);

/**
 * bd_lvm_get_lv_physical_size:
 * @lv_size: LV size
 * @pe_size: PE size
 *
 * Returns: space taken on disk(s) by the LV with given @size
 *
 * Gives number of bytes needed for an LV with the size @lv_size on an LVM stack
 * using given @pe_size.
 */
guint64 bd_lvm_get_lv_physical_size (guint64 lv_size, guint64 pe_size);

/**
 * bd_lvm_get_thpool_padding:
 * @size: size of the thin pool
 * @pe_size: PE size or 0 if the default value should be used
 * @included: if padding is already included in the size
 *
 * Returns: size of the padding needed for a thin pool with the given @size
 *         according to the @pe_size and @included
 */
guint64 bd_lvm_get_thpool_padding (guint64 size, guint64 pe_size, gboolean included);

/**
 * bd_lvm_is_valid_thpool_md_size:
 * @size: the size to be tested
 *
 * Returns: whether the given size is a valid thin pool metadata size or not
 */
gboolean bd_lvm_is_valid_thpool_md_size (guint64 size);

/**
 * bd_lvm_is_valid_thpool_chunk_size:
 * @size: the size to be tested
 * @discard: whether discard/TRIM is required to be supported or not
 *
 * Returns: whether the given size is a valid thin pool chunk size or not
 */
gboolean bd_lvm_is_valid_thpool_chunk_size (guint64 size, gboolean discard);

/**
 * bd_lvm_pvcreate:
 * @device: the device to make PV from
 * @error_message: (out): variable to store error message to (if any)
 *
 * Returns: whether the PV was successfully created or not
 */
gboolean bd_lvm_pvcreate (gchar *device, gchar **error_message);

/**
 * bd_lvm_pvresize:
 * @device: the device to resize
 * @size: the new requested size of the PV or 0 if it should be adjusted to device's size
 * @error_message: (out): variable to store error message to (if any)
 *
 * Returns: whether the PV's size was successfully changed or not
 *
 * If given @size different from 0, sets the PV's size to the given value (see
 * pvresize(8)). If given @size 0, adjusts the PV's size to the underlaying
 * block device's size.
 */
gboolean bd_lvm_pvresize (gchar *device, guint64 size, gchar **error_message);

/**
 * bd_lvm_pvremove:
 * @device: the PV device to be removed/destroyed
 * @error_message: (out): variable to store error message to (if any)
 *
 * Returns: whether the PV was successfully removed/destroyed or not
 */
gboolean bd_lvm_pvremove (gchar *device, gchar **error_message);

/**
 * bd_lvm_pvmove:
 * @src: the PV device to move extents off of
 * @dest: (allow-none): the PV device to move extents onto or #NULL
 * @error_message: (out): variable to store error message to (if any)
 *
 * Returns: whether the extents from the @src PV where successfully movd or not
 *
 * If @dest is #NULL, VG allocation rules are used for the extents from the @src
 * PV (see pvmove(8)).
 */
gboolean bd_lvm_pvmove (gchar *src, gchar *dest, gchar **error_message);

/**
 * bd_lvm_pvscan:
 * @device: the device to scan for PVs
 * @update_cache: whether to update the lvmetad cache or not
 * @error_message: (out): variable to store error message to (if any)
 *
 * Returns: whether the @device was successfully scanned for PVs or not
 */
gboolean bd_lvm_pvscan (gchar *device, gboolean update_cache, gchar **error_message);

/**
 * bd_lvm_pvinfo:
 * @device: a PV to get information about or #NULL
 * @error_message: (out): variable to store error message to (if any)
 *
 * Returns: (transfer full): information about the PV on the given @device or
 * #NULL in case of error (the @error_message gets populated in those cases)
 */
BDLVMPVdata* bd_lvm_pvinfo (gchar *device, gchar **error_message);

/**
 * bd_lvm_vgcreate:
 * @name: name of the newly created VG
 * @pv_list: (array zero-terminated=1): list of PVs the newly created VG should use
 * @pe_size: PE size or 0 if the default value should be used
 * @error_message: (out): variable to store error message to (if any)
 *
 * Returns: whether the VG @name was successfully created or not
 */
gboolean bd_lvm_vgcreate (gchar *name, gchar **pv_list, guint64 pe_size, gchar **error_message);

/**
 * bd_lvm_vgremove:
 * @vg_name: name of the to be removed VG
 * @error_message: (out): variable to store error message to (if any)
 *
 * Returns: whether the VG was successfully removed or not
 *
 * Note: forces the VG removal.
 */
gboolean bd_lvm_vgremove (gchar *vg_name, gchar **error_message);

/**
 * bd_lvm_vgactivate:
 * @vg_name: name of the to be activated VG
 * @error_message: (out): variable to store error message to (if any)
 *
 * Returns: whether the VG was successfully activated or not
 */
gboolean bd_lvm_vgactivate (gchar *vg_name, gchar **error_message);

/**
 * bd_lvm_vgdeactivate:
 * @vg_name: name of the to be deactivated VG
 * @error_message: (out): variable to store error message to (if any)
 *
 * Returns: whether the VG was successfully deactivated or not
 */
gboolean bd_lvm_vgdeactivate (gchar *vg_name, gchar **error_message);

/**
 * bd_lvm_vgextend:
 * @vg_name: name of the to be extended VG
 * @device: PV device to extend the @vg_name VG with
 * @error_message: (out): variable to store error message to (if any)
 *
 * Returns: whether the VG @vg_name was successfully extended with the given @device or not.
 */
gboolean bd_lvm_vgextend (gchar *vg_name, gchar *device, gchar **error_message);

#endif  /* BD_LVM_API */
