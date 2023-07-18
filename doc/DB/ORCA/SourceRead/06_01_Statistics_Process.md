```C++
// 适用于窗口函数
CStatistics * CProjectStatsProcessor::CalcProjStats(CMemoryPool *mp,
		const CStatistics *input_stats, ULongPtrArray *projection_colids,
          UlongToIDatumMap *datum_map) {
	CColumnFactory *col_factory = COptCtxt::PoctxtFromTLS()->Pcf();
	// 构造最终用于生成最终统计信息的hashmap
    // 		<colid, Histogram> hash map -> histograms_new
    // 		<colid, width> hashmap 	    -> colid_width_mapping
	UlongToHistogramMap *histograms_new = GPOS_NEW(mp) UlongToHistogramMap(mp);
	UlongToDoubleMap *colid_width_mapping = GPOS_NEW(mp) UlongToDoubleMap(mp);

	const ULONG length = projection_colids->Size();
	for (ULONG ul = 0; ul < length; ul++) {
		ULONG colid = *(*projection_colids)[ul];
		const CHistogram *histogram = input_stats->GetHistogram(colid);

         // 1. 以input_stats和datum_map为输入,构造输出的Histogram
         //    并插入<colid, Histogram> histograms_new HashMap用于最生成直方图
		if (nullptr == histogram) { // 在输入统计信息中未查找到相应直方图 
             // 为project列创建histogram
			CBucketArray *proj_col_bucket = GPOS_NEW(mp) CBucketArray(mp);
			CDouble null_freq = 0.0;
			BOOL is_well_defined = false;
			if (nullptr != datum_map) {
				IDatum *datum = datum_map->Find(&colid);
				if (nullptr != datum) {
					is_well_defined = true;
					if (!datum->IsNull()) {
						proj_col_bucket->Append(CBucket::MakeBucketSingleton(mp, datum));
					} else {
						null_freq = 1.0;
					}
				}
			}

			CHistogram *proj_col_histogram = nullptr;
			CColRef *colref = col_factory->LookupColRef(colid);
			if (0 == proj_col_bucket->Size() &&
				IMDType::EtiBool == colref->RetrieveType()->GetDatumType()) {
				proj_col_bucket->Release();
				proj_col_histogram = CHistogram::MakeDefaultBoolHistogram(mp);
			} else {
				proj_col_histogram = GPOS_NEW(mp)
					CHistogram(mp, proj_col_bucket, is_well_defined, null_freq,
							   CHistogram::DefaultNDVRemain,
							   CHistogram::DefaultNDVFreqRemain);
			}
			histograms_new->Insert(GPOS_NEW(mp) ULONG(colid), proj_col_histogram);
		} else {
             // 在输入统计信息中查找到相应的直方图, 复制
			histograms_new->Insert(GPOS_NEW(mp) ULONG(colid),
								   histogram->CopyHistogram());
		}

		// 2. 以input_stats为输入, 构造插入<colid, width> hashmap
		const CDouble *width = input_stats->GetWidth(colid);
		if (nullptr == width) {
			CColRef *colref = col_factory->LookupColRef(colid);
			CDouble width = CStatisticsUtils::DefaultColumnWidth(colref->RetrieveType());
			colid_width_mapping->Insert(GPOS_NEW(mp) ULONG(colid),
										GPOS_NEW(mp) CDouble(width));
		} else {
			colid_width_mapping->Insert(GPOS_NEW(mp) ULONG(colid),
                                        GPOS_NEW(mp) CDouble(*width));
		}
	}

	CDouble input_rows = input_stats->Rows();
	// 3. 利用上面生成的<colid, Histogram> hash map histograms_new和
    //     <colid, width> hashmap colid_width_mapping构造最终的统计信息输出
	CStatistics *projection_stats = GPOS_NEW(mp) CStatistics(
		mp, histograms_new, colid_width_mapping, input_rows,
		input_stats->IsEmpty(), input_stats->GetNumberOfPredicates());

	// In the output statistics object, the upper bound source cardinality of the project column
	// is equivalent the estimate project cardinality.
	CStatisticsUtils::ComputeCardUpperBounds(
		mp, input_stats, projection_stats, input_rows,
		CStatistics::EcbmInputSourceMaxCard /* card_bounding_method */);

	// add upper bound card information for the project columns
	CStatistics::CreateAndInsertUpperBoundNDVs(mp, projection_stats,
											   projection_colids, input_rows);

	return projection_stats;
}
```

