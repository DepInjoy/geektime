`bool`默认直方图

```C++
CHistogram * CHistogram::MakeDefaultBoolHistogram(CMemoryPool *mp) {
	CBucketArray *buckets = GPOS_NEW(mp) CBucketArray(mp);
	// a boolean column can at most have 3 values (true, false, and NULL).
	CDouble distinct_remaining = CDouble(3.0);
	CDouble freq_remaining = CDouble(1.0);
	CDouble null_freq = CDouble(0.0);

	return GPOS_NEW(mp) CHistogram(
		mp, buckets, true /* is_well_defined */, null_freq, distinct_remaining,
		freq_remaining, true /*is_col_stats_missing */
	);
}

```

