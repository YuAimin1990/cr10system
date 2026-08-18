SECTIONS
{ 
  . = 0x800000;
  . = ALIGN(4);
  __hytest_start = .;
  .__test_camera_case__list :
  {
      KEEP(*(SORT(.__test_camera_case__list_*)));
  }

  . = ALIGN(4);
  __hytest_end = .;
}
INSERT BEFORE .bss;
